/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateEAC2Command.h"

#include "GlobalStatus.h"
#include "apdu/CommandApdu.h"
#include "apdu/GeneralAuthenticateResponse.h"
#include "asn1/ASN1Struct.h"
#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/EFCardSecurity.h"
#include "pace/ec/EcUtil.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


DidAuthenticateEAC2Command::DidAuthenticateEAC2Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const CVCertificateChain& pCvcChain, const QByteArray& pEphemeralPublicKeyAsHex,
		const QByteArray& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary,
		const QByteArray& pPin)
	: BaseCardCommand(pCardConnectionWorker)
	, mCvcChain(pCvcChain)
	, mEphemeralPublicKeyAsHex(pEphemeralPublicKeyAsHex)
	, mSignatureAsHex(pSignatureAsHex)
	, mAuthenticatedAuxiliaryDataAsBinary(pAuthenticatedAuxiliaryDataAsBinary)
	, mPin(pPin)
	, mEfCardSecurityAsHex()
	, mNonceAsHex()
	, mAuthTokenAsHex()
{
}


void DidAuthenticateEAC2Command::internalExecuteSoftwareSmartCard()
{
	auto [returnCode, efCardSecurity, authenticationToken, nonce] = getCardConnectionWorker()->performTAandCA(
			mCvcChain,
			mAuthenticatedAuxiliaryDataAsBinary,
			QByteArray::fromHex(mSignatureAsHex),
			mPin,
			QByteArray::fromHex(mEphemeralPublicKeyAsHex));

	setReturnCode(returnCode);
	if (returnCode != CardReturnCode::OK)
	{
		qCWarning(card) << "Perform terminal and chip authentication failed:" << CardReturnCodeUtil::toGlobalStatus(getReturnCode());
		return;
	}

	mEfCardSecurityAsHex += efCardSecurity.toHex();
	if (EFCardSecurity::decode(efCardSecurity) == nullptr)
	{
		qCCritical(card) << "Cannot parse EF.CardSecurity";
		setReturnCode(CardReturnCode::PROTOCOL_ERROR);
		return;
	}

	mNonceAsHex = nonce.toHex();
	mAuthTokenAsHex = authenticationToken.toHex();
}


void DidAuthenticateEAC2Command::internalExecute()
{
	if (getCardConnectionWorker()->getReaderInfo().isSoftwareSmartEid())
	{
		internalExecuteSoftwareSmartCard();
		return;
	}

	setReturnCode(putCertificateChain(mCvcChain));
	if (getReturnCode() != CardReturnCode::OK)
	{
		return;
	}

	Oid taProtocol = mCvcChain.getTerminalCvc()->getBody().getPublicKey().getOid();
	QByteArray chr = mCvcChain.getTerminalCvc()->getBody().getCertificateHolderReference();
	QByteArray ephemeralPublicKey = QByteArray::fromHex(mEphemeralPublicKeyAsHex);
	QByteArray signature = QByteArray::fromHex(mSignatureAsHex);
	setReturnCode(performTerminalAuthentication(taProtocol,
			chr,
			mAuthenticatedAuxiliaryDataAsBinary,
			EcUtil::compressPoint(ephemeralPublicKey),
			signature));

	if (getReturnCode() != CardReturnCode::OK)
	{
		return;
	}

	QByteArray efCardSecurityBytes;
	qCDebug(card) << "Performing Read EF.CardSecurity";
	setReturnCode(getCardConnectionWorker()->readFile(FileRef::efCardSecurity(), efCardSecurityBytes, CommandApdu::EXTENDED_MAX_LE));
	if (getReturnCode() != CardReturnCode::OK)
	{
		qCCritical(card) << "Cannot read EF.CardSecurity";
		return;
	}
	mEfCardSecurityAsHex += efCardSecurityBytes.toHex();
	QSharedPointer<EFCardSecurity> efCardSecurity = EFCardSecurity::decode(efCardSecurityBytes);
	if (efCardSecurity == nullptr)
	{
		qCCritical(card) << "Cannot parse EF.CardSecurity";
		setReturnCode(CardReturnCode::PROTOCOL_ERROR);
		return;
	}

	const auto& chipAuthenticationInfoList = efCardSecurity->getSecurityInfos()->getChipAuthenticationInfos();
	if (chipAuthenticationInfoList.isEmpty())
	{
		qCCritical(card) << "No ChipAuthenticationInfo found in EF.CardSecurity";
		setReturnCode(CardReturnCode::PROTOCOL_ERROR);
		return;
	}

	for (const auto& info : chipAuthenticationInfoList)
	{
		if (info->getVersion() == 2)
		{
			qCDebug(card) << "Choose ChipAuthenticationInfo:" << info;
			setReturnCode(performChipAuthentication(info, ephemeralPublicKey));
			return;
		}
	}

	qCCritical(card) << "No ChipAuthenticationInfo with version 2 found in EF.CardSecurity";
	setReturnCode(CardReturnCode::PROTOCOL_ERROR);
}


CardReturnCode DidAuthenticateEAC2Command::putCertificateChain(const CVCertificateChain& pCvcChain)
{
	for (const auto& cvCertificate : pCvcChain)
	{
		QByteArray car = cvCertificate->getBody().getCertificationAuthorityReference();
		ASN1Struct cmdDataSet;
		cmdDataSet.append(ASN1Struct::PUBLIC_KEY_REFERENCE, car);
		CommandApdu cmdApduSet(Ins::MSE_SET, CommandApdu::TERMINAL_AUTHENTICATION, CommandApdu::DIGITAL_SIGNATURE_TEMPLATE, cmdDataSet);

		qCDebug(card) << "Performing TA MSE:Set DST with CAR" << car;
		const auto& [mseReturnCode, mseResult] = getCardConnectionWorker()->transmit(cmdApduSet);
		if (mseReturnCode != CardReturnCode::OK)
		{
			qCWarning(card) << "TA MSE:Set DST failed.";
			return mseReturnCode;
		}

		if (mseResult.getStatusCode() != StatusCode::SUCCESS)
		{
			qCWarning(card) << "TA MSE:Set DST failed:" << mseResult.getStatusCode();
			return CardReturnCode::PROTOCOL_ERROR;
		}

		ASN1Struct cmdDataVerify;
		cmdDataVerify.append(cvCertificate->getRawBody());
		cmdDataVerify.append(cvCertificate->getRawSignature());
		CommandApdu cmdApduVerify(Ins::PSO_VERIFY, CommandApdu::IMPLICIT, CommandApdu::SELF_DESCRIPTIVE, cmdDataVerify);

		qCDebug(card) << "Performing TA PSO:Verify Certificate with" << *cvCertificate;
		auto [psoReturnCode, psoResult] = getCardConnectionWorker()->transmit(cmdApduVerify);
		if (psoReturnCode != CardReturnCode::OK)
		{
			return psoReturnCode;
		}

		switch (psoResult.getStatusCode())
		{
			case StatusCode::SUCCESS:
				break;

			case StatusCode::WRONG_LENGTH:
				return CardReturnCode::WRONG_LENGTH;

			default:
				qCWarning(card) << "TA PSO:Verify Certificate failed:" << psoResult.getStatusCode();
				return CardReturnCode::PROTOCOL_ERROR;
		}
	}

	return CardReturnCode::OK;
}


CardReturnCode DidAuthenticateEAC2Command::performTerminalAuthentication(const Oid& pTaProtocol,
		const QByteArray& pChr,
		const QByteArray& pAuxiliaryData,
		const QByteArray& pCompressedEphemeralPublicKey,
		const QByteArray& pSignature)
{
	ASN1Struct cmdData;
	cmdData.append(ASN1Struct::CRYPTOGRAPHIC_MECHANISM_REFERENCE, pTaProtocol);
	cmdData.append(ASN1Struct::PUBLIC_KEY_REFERENCE, pChr);
	cmdData.append(pAuxiliaryData);
	cmdData.append(ASN1Struct::TA_EPHEMERAL_PUBLIC_KEY, pCompressedEphemeralPublicKey);
	CommandApdu cmdApdu(Ins::MSE_SET, CommandApdu::TERMINAL_AUTHENTICATION, CommandApdu::AUTHENTICATION_TEMPLATE, cmdData);

	qCDebug(card) << "Performing TA MSE:Set AT";
	const auto& [mseReturnCode, mseResult] = getCardConnectionWorker()->transmit(cmdApdu);
	if (mseReturnCode != CardReturnCode::OK)
	{
		qCWarning(card) << "TA MSE:Set AT failed:" << CardReturnCodeUtil::toGlobalStatus(mseReturnCode);
		return mseReturnCode;
	}

	if (mseResult.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "TA MSE:Set AT failed:" << mseResult.getStatusCode();
		return CardReturnCode::PROTOCOL_ERROR;
	}

	CommandApdu externalAuthenticateCmd(Ins::EXTERNAL_AUTHENTICATE, CommandApdu::IMPLICIT, CommandApdu::IMPLICIT, pSignature);
	qCDebug(card) << "Performing TA External Authenticate";
	auto [eaReturnCode, eaResult] = getCardConnectionWorker()->transmit(externalAuthenticateCmd);
	if (eaReturnCode != CardReturnCode::OK)
	{
		qCWarning(card) << "TA External Authenticate failed:" << CardReturnCodeUtil::toGlobalStatus(eaReturnCode);
		return eaReturnCode;
	}

	if (eaResult.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "TA External Authenticate failed:" << eaResult.getStatusCode();
		return CardReturnCode::PROTOCOL_ERROR;
	}

	return CardReturnCode::OK;
}


CardReturnCode DidAuthenticateEAC2Command::performChipAuthentication(QSharedPointer<const ChipAuthenticationInfo> pChipAuthInfo,
		const QByteArray& pEphemeralPublicKey)
{
	ASN1Struct cmdDataSet;
	cmdDataSet.append(ASN1Struct::CRYPTOGRAPHIC_MECHANISM_REFERENCE, pChipAuthInfo->getOid());
	if (pChipAuthInfo->hasKeyId())
	{
		cmdDataSet.append(ASN1Struct::PRIVATE_KEY_REFERENCE, pChipAuthInfo->getKeyId());
	}
	CommandApdu cmdApduSet(Ins::MSE_SET, CommandApdu::CHIP_AUTHENTICATION, CommandApdu::AUTHENTICATION_TEMPLATE, cmdDataSet);

	qCDebug(card) << "Performing CA MSE:Set AT";
	const auto& [mseReturnCode, mseResult] = getCardConnectionWorker()->transmit(cmdApduSet);
	if (mseReturnCode != CardReturnCode::OK)
	{
		return mseReturnCode;
	}

	if (mseResult.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "CA MSE:Set AT failed:" << mseResult.getStatusCode();
		return CardReturnCode::PROTOCOL_ERROR;
	}

	ASN1Struct cmdDataCa(V_ASN1_APPLICATION, ASN1Struct::DYNAMIC_AUTHENTICATION_DATA);
	cmdDataCa.append(ASN1Struct::CA_EPHEMERAL_PUBLIC_KEY, pEphemeralPublicKey);
	CommandApdu cmdApduCa(Ins::GENERAL_AUTHENTICATE, 0, 0, cmdDataCa, CommandApdu::SHORT_MAX_LE);
	qCDebug(card) << "Performing CA General Authenticate";
	const auto& [gaReturnCode, gaGenericResponse] = getCardConnectionWorker()->transmit(cmdApduCa);
	if (gaReturnCode != CardReturnCode::OK)
	{
		return gaReturnCode;
	}

	if (gaGenericResponse.getStatusCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "CA General Authenticate failed:" << gaGenericResponse.getStatusCode();
		return CardReturnCode::PROTOCOL_ERROR;
	}

	const GAChipAuthenticationResponse gaResponse(gaGenericResponse);
	mNonceAsHex = gaResponse.getNonce().toHex();
	mAuthTokenAsHex = gaResponse.getAuthenticationToken().toHex();

	if (mNonceAsHex.isNull() || mAuthTokenAsHex.isNull())
	{
		return CardReturnCode::PROTOCOL_ERROR;
	}

	return CardReturnCode::OK;
}
