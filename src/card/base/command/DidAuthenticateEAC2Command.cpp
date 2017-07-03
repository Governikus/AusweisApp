/*!
 * DidAuthenticateEAC2Command.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "CardConnection.h"
#include "DidAuthenticateEAC2Command.h"
#include "GeneralAuthenticateResponse.h"
#include "ReturnCodeUtil.h"
#include "asn1/ChipAuthenticationInfo.h"
#include "asn1/EFCardSecurity.h"


#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


DidAuthenticateEAC2Command::DidAuthenticateEAC2Command(QSharedPointer<CardConnectionWorker> pCardConnectionWorker,
		const CVCertificateChain& pCvcChain, const QString& pEphemeralPublicKeyAsHex,
		const QString& pSignatureAsHex, const QByteArray& pAuthenticatedAuxiliaryDataAsBinary)
	: BaseCardCommand(pCardConnectionWorker)
	, mCvcChain(pCvcChain)
	, mEphemeralPublicKeyAsHex(pEphemeralPublicKeyAsHex)
	, mSignatureAsHex(pSignatureAsHex)
	, mAuthenticatedAuxiliaryDataAsBinary(pAuthenticatedAuxiliaryDataAsBinary)
	, mEfCardSecurityAsHex()
	, mNonceAsHex()
	, mAuthTokenAsHex()
{
}


DidAuthenticateEAC2Command::~DidAuthenticateEAC2Command()
{

}


void DidAuthenticateEAC2Command::internalExecute()
{
	mReturnCode = putCertificateChain(mCvcChain);
	if (mReturnCode != ReturnCode::OK)
	{
		return;
	}

	QByteArray taProtocol = mCvcChain.getTerminalCvc()->getBody().getPublicKey().getPublicKeyOidValueBytes();
	QByteArray chr = mCvcChain.getTerminalCvc()->getBody().getCertificateHolderReference();

	QByteArray ephemeralPublicKey = QByteArray::fromHex(mEphemeralPublicKeyAsHex.toLatin1());
	if (ephemeralPublicKey.size() % 2 == 0)
	{
		/*
		 * According to TR-03111, chapter 3.2.1 the uncompressed encoding of an elliptic curve point is:
		 *  0x04 | xCoordinate | yCoordinate
		 * In contrast the AGETO server just sends xCoordinate | yCoordinate.
		 *
		 * We fix this by prepending 0x04
		 */
		ephemeralPublicKey.prepend(0x04);
	}
	QByteArray compressedEphemeralPublicKey = ephemeralPublicKey.mid(1, (ephemeralPublicKey.size() - 1) / 2);

	QByteArray signature = QByteArray::fromHex(mSignatureAsHex.toLatin1());
	mReturnCode = performTerminalAuthentication(taProtocol,
			chr,
			mAuthenticatedAuxiliaryDataAsBinary,
			compressedEphemeralPublicKey,
			signature);

	if (mReturnCode != ReturnCode::OK)
	{
		return;
	}


	QByteArray efCardSecurityBytes;
	qCDebug(card) << "Performing Read EF.CardSecurity";
	mReturnCode = mCardConnectionWorker->readFile(FileRef::efCardSecurity(), efCardSecurityBytes);
	if (mReturnCode != ReturnCode::OK)
	{
		return;
	}
	mEfCardSecurityAsHex.append(efCardSecurityBytes.toHex());
	QSharedPointer<EFCardSecurity> efCardSecurity = EFCardSecurity::decode(efCardSecurityBytes);
	if (efCardSecurity == nullptr)
	{
		qCCritical(card) << "Cannot parse EF.CardSecurity";
		mReturnCode = ReturnCode::PROTOCOL_ERROR;
		return;
	}

	QVector<QSharedPointer<ChipAuthenticationInfo> > chipAuthenticationInfoList = efCardSecurity->getSecurityInfos<ChipAuthenticationInfo>();
	if (chipAuthenticationInfoList.isEmpty())
	{
		qCCritical(card) << "No ChipAuthenticationInfo found in EF.CardAccess";
		mReturnCode = ReturnCode::PROTOCOL_ERROR;
		return;
	}
	// we do not know of any procedural rule to determine the ChipAuthenticationInfo, so we just take the first one
	QSharedPointer<ChipAuthenticationInfo> chipAuthenticationInfo = chipAuthenticationInfoList.at(0);
	qCDebug(card) << "Chose ChipAuthenticationInfo(0): protocol" << chipAuthenticationInfo->getProtocol() << ", keyId" << chipAuthenticationInfo->getKeyId().toHex();

	mReturnCode = performChipAuthentication(chipAuthenticationInfo, ephemeralPublicKey, mNonceAsHex, mAuthTokenAsHex);
	if (ReturnCode::OK == mReturnCode)
	{
		mCardConnectionWorker->stopSecureMessaging();
	}
	return;
}


ReturnCode DidAuthenticateEAC2Command::putCertificateChain(const CVCertificateChain& pCvcChain)
{
	for (const auto& cvCertificate : pCvcChain)
	{
		QByteArray car = cvCertificate->getBody().getCertificationAuthorityReference();
		MSEBuilder mseBuilder(MSEBuilder::P1::SET_DST, MSEBuilder::P2::COMPUTE_DIGITAL_SIGNATURE);
		mseBuilder.setPublicKey(car);

		ResponseApdu mseResult;
		qCDebug(card) << "Performing TA MSE:Set DST";
		qCDebug(card) << "Sending CAR" << car;
		ReturnCode returnCode = mCardConnectionWorker->transmit(mseBuilder.build(), mseResult);
		if (returnCode != ReturnCode::OK)
		{
			qCWarning(card) << "TA MSE:Set DST failed.";
			return returnCode;
		}

		if (mseResult.getReturnCode() != StatusCode::SUCCESS)
		{
			qCWarning(card) << "TA MSE:Set DST failed: " << mseResult.getReturnCode();
			return ReturnCode::PROTOCOL_ERROR;
		}

		PSOBuilder psoBuilder(PSOBuilder::P1::VERIFY, PSOBuilder::P2::CERTIFICATE);
		psoBuilder.setCertificateBody(cvCertificate->getRawBody(), true);
		psoBuilder.setSignature(cvCertificate->getRawSignature(), true);

		qCDebug(card) << "Performing TA PSO:Verify Certificate";
		qCDebug(card) << "Sending certificate" << *cvCertificate;
		ResponseApdu psoResult;
		returnCode = mCardConnectionWorker->transmit(psoBuilder.build(), psoResult);
		if (returnCode != ReturnCode::OK)
		{
			return returnCode;
		}

		if (psoResult.getReturnCode() != StatusCode::SUCCESS)
		{
			qCWarning(card) << "TA PSO:Verify Certificate failed: " << psoResult.getReturnCode();
			return ReturnCode::PROTOCOL_ERROR;
		}
	}

	return ReturnCode::OK;
}


ReturnCode DidAuthenticateEAC2Command::performTerminalAuthentication(const QByteArray& taProtocol,
		const QByteArray& chr,
		const QByteArray& auxiliaryData,
		const QByteArray& compressedEphemeralPublicKey,
		const QByteArray& signature)
{

	MSEBuilder mseBuilder(MSEBuilder::P1::SET_DST, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(taProtocol);
	mseBuilder.setPublicKey(chr);
	mseBuilder.setAuxiliaryData(auxiliaryData, true);
	mseBuilder.setEphemeralPublicKey(compressedEphemeralPublicKey);

	ResponseApdu mseResult;
	qCDebug(card) << "Performing TA MSE:Set AT";
	ReturnCode returnCode = mCardConnectionWorker->transmit(mseBuilder.build(), mseResult);
	if (returnCode != ReturnCode::OK)
	{
		qCWarning(card) << "TA MSE:Set AT failed: " << ReturnCodeUtil::toString(returnCode);
		return returnCode;
	}
	if (mseResult.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "TA MSE:Set AT failed: " << mseResult.getReturnCode();
		return ReturnCode::PROTOCOL_ERROR;
	}

	ResponseApdu eaResult;
	EABuilder eaBuilder;
	eaBuilder.setSignature(signature);
	qCDebug(card) << "Performing TA External Authenticate";
	returnCode = mCardConnectionWorker->transmit(eaBuilder.build(), eaResult);
	if (returnCode != ReturnCode::OK)
	{
		qCWarning(card) << "TA External Authenticate failed: " << ReturnCodeUtil::toString(returnCode);
		return returnCode;
	}
	if (eaResult.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "TA External Authenticate failed: " << eaResult.getReturnCode();
		return ReturnCode::PROTOCOL_ERROR;
	}

	return ReturnCode::OK;
}


ReturnCode DidAuthenticateEAC2Command::performChipAuthentication(QSharedPointer<const ChipAuthenticationInfo> pChipAuthInfo,
		const QByteArray& ephemeralPublicKey,
		QByteArray& pNonceAsHex,
		QByteArray& pAuthTokenAsHex)
{
	ResponseApdu mseResult;
	MSEBuilder mseBuilder(MSEBuilder::P1::COMPUTE_DIGITAL_SIGNATURE, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(pChipAuthInfo->getProtocolValueBytes());
	mseBuilder.setPrivateKey(pChipAuthInfo->getKeyId());

	qCDebug(card) << "Performing CA MSE:Set AT";
	ReturnCode returnCode = mCardConnectionWorker->transmit(mseBuilder.build(), mseResult);
	if (returnCode != ReturnCode::OK)
	{
		return returnCode;
	}
	if (mseResult.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "CA MSE:Set AT failed: " << mseResult.getReturnCode();
		return ReturnCode::PROTOCOL_ERROR;
	}

	GAChipAuthenticationResponse gaResponse;
	GABuilder gaBuilder;
	gaBuilder.setCaEphemeralPublicKey(ephemeralPublicKey);

	qCDebug(card) << "Performing CA General Authenticate";
	returnCode = mCardConnectionWorker->transmit(gaBuilder.build(), gaResponse);
	if (returnCode != ReturnCode::OK)
	{
		return returnCode;
	}
	if (gaResponse.getReturnCode() != StatusCode::SUCCESS)
	{
		qCWarning(card) << "CA General Authenticate failed: " << gaResponse.getReturnCode();
		return ReturnCode::PROTOCOL_ERROR;
	}
	pNonceAsHex.append(gaResponse.getNonce().toHex());
	pAuthTokenAsHex.append(gaResponse.getAuthenticationToken().toHex());

	return ReturnCode::OK;
}
