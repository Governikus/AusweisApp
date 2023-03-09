/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */


#include "KeyAgreement.h"

#include "GlobalStatus.h"
#include "apdu/CommandApdu.h"
#include "apdu/CommandData.h"
#include "asn1/PaceInfo.h"
#include "pace/CipherMac.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/SymmetricCipher.h"
#include "pace/ec/EcdhKeyAgreement.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


static QString getResponseErrorString(CardReturnCode pReturnCode, StatusCode pResponseReturnCode)
{
	return CardReturnCodeUtil::toGlobalStatus(pReturnCode).toErrorDescription() + QStringLiteral(" | ") + pResponseReturnCode;
}


KeyAgreement::CardResult KeyAgreement::createTransmitResult(CardReturnCode pReturnCode,
		const ResponseApdu& pResponseApdu,
		const QByteArray& pResultData,
		const char* pLogMessage) const
{
	if (pReturnCode == CardReturnCode::OK && pResponseApdu.getStatusCode() == StatusCode::SUCCESS)
	{
		return {pReturnCode, pResultData};
	}

	CardReturnCode newReturnCode = CardReturnCode::COMMAND_FAILED;
	if (pResponseApdu.isEmpty())
	{
		newReturnCode = CardReturnCode::RETRY_ALLOWED;
	}
	else if (pReturnCode == CardReturnCode::OK)
	{
		newReturnCode = CardReturnCode::PROTOCOL_ERROR;
	}

	qCCritical(card).noquote() << pLogMessage << getResponseErrorString(newReturnCode, pResponseApdu.getStatusCode());
	return {newReturnCode};
}


QSharedPointer<KeyAgreement> KeyAgreement::create(const QSharedPointer<const PaceInfo>& pPaceInfo,
		QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
{
	if (pPaceInfo->getProtocol().getKeyAgreement() == KeyAgreementType::ECDH)
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


KeyAgreement::~KeyAgreement() = default;


KeyAgreementStatus KeyAgreement::perform(const QByteArray& pPin)
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
			break;
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
			break;
	}

	mEncryptionKey = mKeyDerivationFunction.enc(sharedSecret);
	mMacKey = mKeyDerivationFunction.mac(sharedSecret);

	return performMutualAuthenticate();
}


KeyAgreement::CardResult KeyAgreement::determineNonce(const QByteArray& pPin)
{
	const auto result = transmitGAEncryptedNonce();
	if (result.mReturnCode != CardReturnCode::OK)
	{
		return result;
	}

	const auto symmetricKey = mKeyDerivationFunction.pi(pPin);
	SymmetricCipher nonceDecrypter(mPaceInfo->getProtocol(), symmetricKey);

	return {CardReturnCode::OK, nonceDecrypter.decrypt(result.mData)};
}


KeyAgreementStatus KeyAgreement::performMutualAuthenticate()
{
	CipherMac cmac(mPaceInfo->getProtocol(), mMacKey);

	QByteArray uncompressedCardPublicKey = getUncompressedCardPublicKey();
	QByteArray mutualAuthenticationCardData = cmac.generate(uncompressedCardPublicKey);

	const GAMutualAuthenticationResponse response = transmitGAMutualAuthentication(mutualAuthenticationCardData);
	if (response.isEmpty())
	{
		return KeyAgreementStatus::RETRY_ALLOWED;
	}

	if (response.getRetryCounter() < 3)
	{
		if (response.getSW1() == SW1::NONVOLATILE_MEMORY_CHANGED_1)
		{
			return KeyAgreementStatus::FAILED;
		}

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
	CommandData cmdData(V_ASN1_APPLICATION, CommandData::DYNAMIC_AUTHENTICATION_DATA);
	CommandApdu cmdApdu(Ins::GENERAL_AUTHENTICATE, 0, 0, cmdData, CommandApdu::SHORT_MAX_LE);
	cmdApdu.enableCommandChaining();

	const auto& [returnCode, responseApdu] = mCardConnectionWorker->transmit(cmdApdu);
	const GAEncryptedNonceResponse response(responseApdu);
	return createTransmitResult(returnCode, responseApdu, response.getEncryptedNonce(), "Error on GA (Encrypted Nonce):");
}


KeyAgreement::CardResult KeyAgreement::transmitGAEphemeralPublicKey(const QByteArray& pEphemeralPublicKey)
{
	CommandData cmdData(V_ASN1_APPLICATION, CommandData::DYNAMIC_AUTHENTICATION_DATA);
	cmdData.append(CommandData::PACE_EPHEMERAL_PUBLIC_KEY, pEphemeralPublicKey);
	CommandApdu cmdApdu(Ins::GENERAL_AUTHENTICATE, 0, 0, cmdData, CommandApdu::SHORT_MAX_LE);
	cmdApdu.enableCommandChaining();

	const auto& [returnCode, responseApdu] = mCardConnectionWorker->transmit(cmdApdu);
	const GAPerformKeyAgreementResponse response(responseApdu);
	return createTransmitResult(returnCode, responseApdu, response.getEphemeralPublicKey(), "Error on GA(Perform Key Agreement):");
}


KeyAgreement::CardResult KeyAgreement::transmitGAMappingData(const QByteArray& pMappingData) const
{
	// sende den PublicKey (D.3.4.)
	CommandData cmdData(V_ASN1_APPLICATION, CommandData::DYNAMIC_AUTHENTICATION_DATA);
	cmdData.append(CommandData::MAPPING_DATA, pMappingData);
	CommandApdu cmdApdu(Ins::GENERAL_AUTHENTICATE, 0, 0, cmdData, CommandApdu::SHORT_MAX_LE);
	cmdApdu.enableCommandChaining();

	auto [returnCode, responseApdu] = mCardConnectionWorker->transmit(cmdApdu);
	const GAMapNonceResponse response(responseApdu);
	return createTransmitResult(returnCode, responseApdu, response.getMappingData(), "Error on GA(Mapping Data):");
}


GAMutualAuthenticationResponse KeyAgreement::transmitGAMutualAuthentication(const QByteArray& pMutualAuthenticationData)
{
	CommandData cmdData(V_ASN1_APPLICATION, CommandData::DYNAMIC_AUTHENTICATION_DATA);
	cmdData.append(CommandData::AUTHENTICATION_TOKEN, pMutualAuthenticationData);
	CommandApdu cmdApdu(Ins::GENERAL_AUTHENTICATE, 0, 0, cmdData, CommandApdu::SHORT_MAX_LE);

	auto [returnCode, responseApdu] = mCardConnectionWorker->transmit(cmdApdu);
	if (returnCode != CardReturnCode::OK || responseApdu.getStatusCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Mutual Authentication):" << getResponseErrorString(returnCode, responseApdu.getStatusCode());
	}
	return GAMutualAuthenticationResponse(responseApdu);
}


const QSharedPointer<const PaceInfo>& KeyAgreement::getPaceInfo() const
{
	return mPaceInfo;
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
