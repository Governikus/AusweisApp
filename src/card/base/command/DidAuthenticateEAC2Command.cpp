/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "DidAuthenticateEAC2Command.h"

#include "GlobalStatus.h"
#include "apdu/CommandApdu.h"
#include "apdu/CommandData.h"
#include "apdu/GeneralAuthenticateResponse.h"
#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/EFCardSecurity.h"

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

	mReturnCode = returnCode;
	if (mReturnCode != CardReturnCode::OK)
	{
		qCWarning(card) << "Perform terminal and chip authentication failed:" << CardReturnCodeUtil::toGlobalStatus(mReturnCode);
		return;
	}

	mEfCardSecurityAsHex += efCardSecurity.toHex();
	if (EFCardSecurity::decode(efCardSecurity) == nullptr)
	{
		qCCritical(card) << "Cannot parse EF.CardSecurity";
		mReturnCode = CardReturnCode::PROTOCOL_ERROR;
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

	mReturnCode = putCertificateChain(mCvcChain);
	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	Oid taProtocol = mCvcChain.getTerminalCvc()->getBody().getPublicKey().getOid();
	QByteArray chr = mCvcChain.getTerminalCvc()->getBody().getCertificateHolderReference();

	QByteArray ephemeralPublicKey = QByteArray::fromHex(mEphemeralPublicKeyAsHex);
	if (ephemeralPublicKey.size() % 2 == 0)
	{
		/*
		 * According to TR-03111, chapter 3.2.1 the uncompressed encoding of an elliptic curve point is:
		 *  0x04 | xCoordinate | yCoordinate
		 * In contrast the AGETO and mtG server just sends xCoordinate | yCoordinate.
		 *
		 * We fix this by prepending 0x04
		 */
		ephemeralPublicKey.prepend(0x04);
	}
	QByteArray compressedEphemeralPublicKey = ephemeralPublicKey.mid(1, (ephemeralPublicKey.size() - 1) / 2);

	QByteArray signature = QByteArray::fromHex(mSignatureAsHex);
	mReturnCode = performTerminalAuthentication(taProtocol,
			chr,
			mAuthenticatedAuxiliaryDataAsBinary,
			compressedEphemeralPublicKey,
			signature);

	if (mReturnCode != CardReturnCode::OK)
	{
		return;
	}

	QByteArray efCardSecurityBytes;
	qCDebug(card) << "Performing Read EF.CardSecurity";
	mReturnCode = getCardConnectionWorker()->readFile(FileRef::efCardSecurity(), efCardSecurityBytes, CommandApdu::EXTENDED_MAX_LE);
	if (mReturnCode != CardReturnCode::OK)
	{
		qCCritical(card) << "Cannot read EF.CardSecurity";
		return;
	}
	mEfCardSecurityAsHex += efCardSecurityBytes.toHex();
	QSharedPointer<EFCardSecurity> efCardSecurity = EFCardSecurity::decode(efCardSecurityBytes);
	if (efCardSecurity == nullptr)
	{
		qCCritical(card) << "Cannot parse EF.CardSecurity";
		mReturnCode = CardReturnCode::PROTOCOL_ERROR;
		return;
	}

	const auto& chipAuthenticationInfoList = efCardSecurity->getSecurityInfos()->getChipAuthenticationInfos();
	if (chipAuthenticationInfoList.isEmpty())
	{
		qCCritical(card) << "No ChipAuthenticationInfo found in EF.CardSecurity";
		mReturnCode = CardReturnCode::PROTOCOL_ERROR;
		return;
	}

	for (const auto& info : chipAuthenticationInfoList)
	{
		if (info->getVersion() == 2)
		{
			qCDebug(card) << "Choose ChipAuthenticationInfo:" << info;
			mReturnCode = performChipAuthentication(info, ephemeralPublicKey);
			return;
		}
	}

	qCCritical(card) << "No ChipAuthenticationInfo with version 2 found in EF.CardSecurity";
	mReturnCode = CardReturnCode::PROTOCOL_ERROR;
}


CardReturnCode DidAuthenticateEAC2Command::putCertificateChain(const CVCertificateChain& pCvcChain)
{
	for (const auto& cvCertificate : pCvcChain)
	{
		QByteArray car = cvCertificate->getBody().getCertificationAuthorityReference();
		CommandData cmdDataSet;
		cmdDataSet.append(CommandData::PUBLIC_KEY_REFERENCE, car);
		CommandApdu cmdApduSet(Ins::MSE_SET, CommandApdu::VERIFICATION, CommandApdu::DIGITAL_SIGNATURE_TEMPLATE, cmdDataSet);

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

		CommandData cmdDataVerify;
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
				return CardReturnCode::EXTENDED_LENGTH_MISSING;

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
	CommandData cmdData;
	cmdData.append(CommandData::CRYPTOGRAPHIC_MECHANISM_REFERENCE, pTaProtocol);
	cmdData.append(CommandData::PUBLIC_KEY_REFERENCE, pChr);
	cmdData.append(pAuxiliaryData);
	cmdData.append(CommandData::TA_EPHEMERAL_PUBLIC_KEY, pCompressedEphemeralPublicKey);
	CommandApdu cmdApdu(Ins::MSE_SET, CommandApdu::VERIFICATION, CommandApdu::AUTHENTICATION_TEMPLATE, cmdData);

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
	CommandData cmdDataSet;
	cmdDataSet.append(CommandData::CRYPTOGRAPHIC_MECHANISM_REFERENCE, pChipAuthInfo->getOid());
	if (pChipAuthInfo->hasKeyId())
	{
		cmdDataSet.append(CommandData::PRIVATE_KEY_REFERENCE, pChipAuthInfo->getKeyId());
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

	CommandData cmdDataCa(CommandData::DYNAMIC_AUTHENTICATION_DATA);
	cmdDataCa.append(CommandData::CA_EPHEMERAL_PUBLIC_KEY, pEphemeralPublicKey);
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
