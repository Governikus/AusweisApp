/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */


#include "KeyAgreement.h"

#include "asn1/PaceInfo.h"
#include "GABuilder.h"
#include "GlobalStatus.h"
#include "pace/CipherMac.h"
#include "pace/ec/EcdhKeyAgreement.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/SymmetricCipher.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


static QString getResponseErrorString(CardReturnCode pReturnCode, StatusCode pResponseReturnCode)
{
	return CardReturnCodeUtil::toGlobalStatus(pReturnCode).toErrorDescription() + QStringLiteral(" | ") + pResponseReturnCode;
}


KeyAgreement::CardResult KeyAgreement::createTransmitResult(CardReturnCode pReturnCode,
		StatusCode pResponseReturnCode,
		const QByteArray& pResultData,
		const char* pLogMessage)
{
	if (pReturnCode == CardReturnCode::OK && pResponseReturnCode == StatusCode::SUCCESS)
	{
		return {pReturnCode, pResultData};
	}

	CardReturnCode newReturnCode = CardReturnCode::COMMAND_FAILED;
	if (pResponseReturnCode == StatusCode::EMPTY)
	{
		newReturnCode = CardReturnCode::RETRY_ALLOWED;
	}
	else if (pReturnCode == CardReturnCode::OK)
	{
		newReturnCode = CardReturnCode::PROTOCOL_ERROR;
	}

	qCCritical(card).noquote() << pLogMessage << getResponseErrorString(newReturnCode, pResponseReturnCode);
	return {newReturnCode};
}


QSharedPointer<KeyAgreement> KeyAgreement::create(const QSharedPointer<const PaceInfo>& pPaceInfo,
		QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	if (pPaceInfo->getKeyAgreementType() == KeyAgreementType::ECDH)
	{
		return EcdhKeyAgreement::create(pPaceInfo, pCardConnectionWorker);
	}
	else
	{
		qCCritical(card) << "Currently only ECDH key agreement supported";
		return QSharedPointer<KeyAgreement>();
	}
}


KeyAgreement::KeyAgreement(const QSharedPointer<const PaceInfo>& pPaceInfo, const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: mCardConnectionWorker(pCardConnectionWorker)
	, mEncryptionKey()
	, mMacKey()
	, mCarCurr()
	, mCarPrev()
	, mPaceInfo(pPaceInfo)
	, mKeyDerivationFunction(pPaceInfo->getProtocol())
{
}


KeyAgreement::~KeyAgreement()
{
}


KeyAgreementStatus KeyAgreement::perform(const QString& pPin)
{
	auto [returnCode, nonce] = determineNonce(pPin);
	switch (returnCode)
	{
		case CardReturnCode::PROTOCOL_ERROR:
			return KeyAgreementStatus::PROTOCOL_ERROR;

		case CardReturnCode::COMMAND_FAILED:
			return KeyAgreementStatus::COMMUNICATION_ERROR;

		case CardReturnCode::RETRY_ALLOWED:
			return KeyAgreementStatus::RETRY_ALLOWED;

		default:
		{}
	}

	auto [sharedSecretReturnCode, sharedSecret] = determineSharedSecret(nonce);
	switch (sharedSecretReturnCode)
	{
		case CardReturnCode::COMMAND_FAILED:
			return KeyAgreementStatus::COMMUNICATION_ERROR;

		case CardReturnCode::PROTOCOL_ERROR:
			return KeyAgreementStatus::PROTOCOL_ERROR;

		case CardReturnCode::RETRY_ALLOWED:
			return KeyAgreementStatus::RETRY_ALLOWED;

		default:
		{}
	}

	mEncryptionKey = mKeyDerivationFunction.enc(sharedSecret);
	mMacKey = mKeyDerivationFunction.mac(sharedSecret);

	return performMutualAuthenticate();
}


KeyAgreement::CardResult KeyAgreement::determineNonce(const QString& pPin)
{
	const auto result = transmitGAEncryptedNonce();
	if (result.mReturnCode != CardReturnCode::OK)
	{
		return result;
	}

	QByteArray symmetricKey = mKeyDerivationFunction.pi(pPin);
	SymmetricCipher nonceDecrypter(mPaceInfo->getProtocol(), symmetricKey);

	return {CardReturnCode::OK, nonceDecrypter.decrypt(result.mData)};
}


KeyAgreementStatus KeyAgreement::performMutualAuthenticate()
{
	CipherMac cmac(mPaceInfo->getProtocol(), mMacKey);

	QByteArray uncompressedCardPublicKey = getUncompressedCardPublicKey();
	QByteArray mutualAuthenticationCardData = cmac.generate(uncompressedCardPublicKey);

	const GAMutualAuthenticationResponse response = transmitGAMutualAuthentication(mutualAuthenticationCardData);
	if (response.getReturnCode() == StatusCode::EMPTY)
	{
		return KeyAgreementStatus::RETRY_ALLOWED;
	}

	if (response.getReturnCode() == StatusCode::VERIFICATION_FAILED ||
			response.getReturnCode() == StatusCode::PIN_BLOCKED ||
			response.getReturnCode() == StatusCode::PIN_SUSPENDED ||
			response.getReturnCode() == StatusCode::PIN_RETRY_COUNT_2)
	{
		return KeyAgreementStatus::FAILED;
	}

	if (response.getReturnCode() != StatusCode::SUCCESS)
	{
		return KeyAgreementStatus::PROTOCOL_ERROR;
	}

	QByteArray uncompressedTerminalPublicKey = getUncompressedTerminalPublicKey();
	QByteArray mutualAuthenticationTerminalData = cmac.generate(uncompressedTerminalPublicKey);

	if (mutualAuthenticationTerminalData != response.getAuthenticationToken())
	{
		qCCritical(card) << "Error on mutual authentication";
		return KeyAgreementStatus::PROTOCOL_ERROR;
	}

	mCarCurr = response.getCarCurr();
	mCarPrev = response.getCarPrev();
	qCDebug(card) << "Successfully authenticated";
	return KeyAgreementStatus::SUCCESS;
}


KeyAgreement::CardResult KeyAgreement::transmitGAEncryptedNonce()
{
	GABuilder builder(CommandApdu::CLA_COMMAND_CHAINING);
	ResponseApdu responseApdu;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(builder.build(), responseApdu);
	GAEncryptedNonceResponse response(responseApdu);
	return createTransmitResult(returnCode, response.getReturnCode(), response.getEncryptedNonce(), "Error on GA (Encrypted Nonce):");
}


KeyAgreement::CardResult KeyAgreement::transmitGAEphemeralPublicKey(const QByteArray& pEphemeralPublicKey)
{
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceEphemeralPublicKey(pEphemeralPublicKey);
	ResponseApdu responseApdu;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), responseApdu);
	const GAPerformKeyAgreementResponse response(responseApdu);
	return createTransmitResult(returnCode, response.getReturnCode(), response.getEphemeralPublicKey(), "Error on GA(Perform Key Agreement):");
}


KeyAgreement::CardResult KeyAgreement::transmitGAMappingData(const QByteArray& pMappingData)
{
	// sende den PublicKey (D.3.4.)
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceMappingData(pMappingData);
	ResponseApdu responseApdu;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), responseApdu);
	const GAMapNonceResponse response(responseApdu);
	return createTransmitResult(returnCode, response.getReturnCode(), response.getMappingData(), "Error on GA(Mapping Data):");
}


GAMutualAuthenticationResponse KeyAgreement::transmitGAMutualAuthentication(const QByteArray& pMutualAuthenticationData)
{
	GABuilder commandBuilder(CommandApdu::CLA);
	commandBuilder.setPaceAuthenticationToken(pMutualAuthenticationData);
	ResponseApdu response;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), response);
	if (returnCode != CardReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Mutual Authentication):" << getResponseErrorString(returnCode, response.getReturnCode());
	}
	return GAMutualAuthenticationResponse(response);
}


const QByteArray& KeyAgreement::getEncryptionKey() const
{
	return mEncryptionKey;
}


const QByteArray& KeyAgreement::getCarCurr() const
{
	return mCarCurr;
}


const QByteArray& KeyAgreement::getCarPrev() const
{
	return mCarPrev;
}


const QByteArray& KeyAgreement::getMacKey() const
{
	return mMacKey;
}
