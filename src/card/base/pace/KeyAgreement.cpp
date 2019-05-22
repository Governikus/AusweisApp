/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */


#include "KeyAgreement.h"

#include "asn1/PaceInfo.h"
#include "Commands.h"
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


static CardOperationResult<QByteArray> makeTransmitResult(CardReturnCode pReturnCode,
		StatusCode pResponseReturnCode,
		const QByteArray& pResultData,
		const QString& pLogMessage)
{
	if (pReturnCode == CardReturnCode::OK && pResponseReturnCode == StatusCode::SUCCESS)
	{
		return CardOperationResult<QByteArray>(pReturnCode, pResultData);
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
	return CardOperationResult<QByteArray>(newReturnCode, QByteArray());
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
	CardOperationResult<QByteArray> nonceResult = determineNonce(pPin);
	switch (nonceResult.getReturnCode())
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

	QByteArray nonce = nonceResult.getPayload();
	CardOperationResult<QByteArray> sharedSecretResult = determineSharedSecret(nonce);
	switch (sharedSecretResult.getReturnCode())
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

	QByteArray sharedSecret = sharedSecretResult.getPayload();
	mEncryptionKey = mKeyDerivationFunction.enc(sharedSecret);
	mMacKey = mKeyDerivationFunction.mac(sharedSecret);

	return performMutualAuthenticate();
}


CardOperationResult<QByteArray> KeyAgreement::determineNonce(const QString& pPin)
{
	CardOperationResult<QByteArray> result = transmitGAEncryptedNonce();
	if (result.getReturnCode() != CardReturnCode::OK)
	{
		return result;
	}

	QByteArray encryptedNonce = result.getPayload();
	QByteArray symmetricKey = mKeyDerivationFunction.pi(pPin);
	SymmetricCipher nonceDecrypter(mPaceInfo->getProtocol(), symmetricKey);

	return CardOperationResult<QByteArray>(CardReturnCode::OK, nonceDecrypter.decrypt(encryptedNonce));
}


KeyAgreementStatus KeyAgreement::performMutualAuthenticate()
{
	CipherMac cmac(mPaceInfo->getProtocol(), mMacKey);

	QByteArray uncompressedCardPublicKey = getUncompressedCardPublicKey();
	QByteArray mutualAuthenticationCardData = cmac.generate(uncompressedCardPublicKey);

	QSharedPointer<GAMutualAuthenticationResponse> response = transmitGAMutualAuthentication(mutualAuthenticationCardData);
	if (response->getReturnCode() == StatusCode::EMPTY)
	{
		return KeyAgreementStatus::RETRY_ALLOWED;
	}
	else if (response->getReturnCode() == StatusCode::VERIFICATION_FAILED ||
			response->getReturnCode() == StatusCode::PIN_BLOCKED ||
			response->getReturnCode() == StatusCode::PIN_SUSPENDED ||
			response->getReturnCode() == StatusCode::PIN_RETRY_COUNT_2)
	{
		return KeyAgreementStatus::FAILED;
	}
	else if (response->getReturnCode() != StatusCode::SUCCESS)
	{
		return KeyAgreementStatus::PROTOCOL_ERROR;
	}

	QByteArray uncompressedTerminalPublicKey = getUncompressedTerminalPublicKey();
	QByteArray mutualAuthenticationTerminalData = cmac.generate(uncompressedTerminalPublicKey);

	if (mutualAuthenticationTerminalData != response->getAuthenticationToken())
	{
		qCCritical(card) << "Error on mutual authentication";
		return KeyAgreementStatus::PROTOCOL_ERROR;
	}

	mCarCurr = response->getCarCurr();
	mCarPrev = response->getCarPrev();
	qCDebug(card) << "Successfully authenticated";
	return KeyAgreementStatus::SUCCESS;
}


CardOperationResult<QByteArray> KeyAgreement::transmitGAEncryptedNonce()
{
	GABuilder builder(CommandApdu::CLA_COMMAND_CHAINING);
	GAEncryptedNonceResponse response;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(builder.build(), response);
	return makeTransmitResult(returnCode, response.getReturnCode(), response.getEncryptedNonce(), QStringLiteral("Error on GA (Encrypted Nonce):"));
}


CardOperationResult<QByteArray> KeyAgreement::transmitGAEphemeralPublicKey(const QByteArray& pEphemeralPublicKey)
{
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceEphemeralPublicKey(pEphemeralPublicKey);
	GAPerformKeyAgreementResponse response;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), response);
	return makeTransmitResult(returnCode, response.getReturnCode(), response.getEphemeralPublicKey(), QStringLiteral("Error on GA(Perform Key Agreement):"));
}


CardOperationResult<QByteArray> KeyAgreement::transmitGAMappingData(const QByteArray& pMappingData)
{
	// sende den PublicKey (D.3.4.)
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceMappingData(pMappingData);
	GAMapNonceResponse response;

	const CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), response);
	return makeTransmitResult(returnCode, response.getReturnCode(), response.getMappingData(), QStringLiteral("Error on GA(Mapping Data):"));
}


QSharedPointer<GAMutualAuthenticationResponse> KeyAgreement::transmitGAMutualAuthentication(const QByteArray& pMutualAuthenticationData)
{
	GABuilder commandBuilder(CommandApdu::CLA);
	commandBuilder.setPaceAuthenticationToken(pMutualAuthenticationData);
	auto response = QSharedPointer<GAMutualAuthenticationResponse>::create();

	CardReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), *response);
	if (returnCode != CardReturnCode::OK || response->getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Mutual Authentication):" << getResponseErrorString(returnCode, response->getReturnCode());
	}
	return response;
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
