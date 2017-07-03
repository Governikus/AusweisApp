/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Commands.h"
#include "PersoSimWorkaround.h"
#include "ReturnCodeUtil.h"
#include "asn1/PACEInfo.h"
#include "pace/CipherMac.h"
#include "pace/KeyAgreement.h"
#include "pace/KeyDerivationFunction.h"
#include "pace/SymmetricCipher.h"
#include "pace/ec/EcdhKeyAgreement.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


template<typename ResponseType>
static QString getResponseErrorString(ReturnCode pReturnCode, const ResponseType& pResponse)
{
	QString errorString = ReturnCodeUtil::toString(pReturnCode);
	if (pReturnCode == ReturnCode::OK)
	{
		errorString += QStringLiteral(" | ") + pResponse.getReturnCode();
	}
	return errorString;
}


QSharedPointer<KeyAgreement> KeyAgreement::create(QSharedPointer<PACEInfo> pPaceInfo,
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


KeyAgreement::KeyAgreement(QSharedPointer<PACEInfo> pPaceInfo, QSharedPointer<CardConnectionWorker> pCardConnectionWorker)
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
	QByteArray nonce = determineNonce(pPin);
	if (nonce.isNull())
	{
		return KeyAgreementStatus::PROTOCOLL_ERROR;
	}

	QByteArray sharedSecret = determineSharedSecret(nonce);
	if (sharedSecret.isNull())
	{
		return KeyAgreementStatus::PROTOCOLL_ERROR;
	}

	mEncryptionKey = mKeyDerivationFunction.enc(sharedSecret);
	mMacKey = mKeyDerivationFunction.mac(sharedSecret);

	return performMutualAuthenticate();
}


QByteArray KeyAgreement::determineNonce(const QString& pPin)
{
	QByteArray encryptedNonce = transmitGAEncryptedNonce();
	if (encryptedNonce.isNull())
	{
		return QByteArray();
	}

	QByteArray symmetricKey = mKeyDerivationFunction.pi(pPin);
	SymmetricCipher nonceDecrypter(mPaceInfo->getProtocol(), symmetricKey);

	return nonceDecrypter.decrypt(encryptedNonce);
}


KeyAgreementStatus KeyAgreement::performMutualAuthenticate()
{
	CipherMac cmac(mPaceInfo->getProtocol(), mMacKey);

	QByteArray uncompressedCardPublicKey = getUncompressedCardPublicKey();
	QByteArray mutualAuthenticationCardData = cmac.generate(uncompressedCardPublicKey);

	QSharedPointer<GAMutualAuthenticationResponse> response = transmitGAMutualAuthentication(mutualAuthenticationCardData);
	if (response->getReturnCode() == StatusCode::VERIFICATION_FAILED ||
			response->getReturnCode() == StatusCode::PIN_BLOCKED ||
			response->getReturnCode() == StatusCode::PIN_SUSPENDED ||
			response->getReturnCode() == StatusCode::PIN_RETRY_COUNT_2 ||
			PersoSimWorkaround::isWrongCanEntry(response))
	{
		return KeyAgreementStatus::FAILED;
	}
	else if (response->getReturnCode() != StatusCode::SUCCESS)
	{
		return KeyAgreementStatus::PROTOCOLL_ERROR;
	}

	QByteArray uncompressedTerminalPublicKey = getUncompressedTerminalPublicKey();
	QByteArray mutualAuthenticationTerminalData = cmac.generate(uncompressedTerminalPublicKey);

	if (mutualAuthenticationTerminalData != response->getAuthenticationToken())
	{
		qCCritical(card) << "Error on mutual authentication ";
		return KeyAgreementStatus::PROTOCOLL_ERROR;
	}

	mCarCurr = response->getCarCurr();
	mCarPrev = response->getCarPrev();
	qCDebug(card) << "Successfully authenticated";
	return KeyAgreementStatus::SUCCESS;
}


QByteArray KeyAgreement::transmitGAEncryptedNonce()
{
	GABuilder builder(CommandApdu::CLA_COMMAND_CHAINING);
	GAEncryptedNonceResponse response;

	ReturnCode returnCode = mCardConnectionWorker->transmit(builder.build(), response);
	if (returnCode != ReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA (Encrypted Nonce):" << getResponseErrorString(returnCode, response);
		return QByteArray();
	}
	return response.getEncryptedNonce();
}


QByteArray KeyAgreement::transmitGAEphemeralPublicKey(const QByteArray& pEphemeralPublicKey)
{
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceEphemeralPublicKey(pEphemeralPublicKey);
	GAPerformKeyAgreementResponse response;

	ReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), response);
	if (returnCode != ReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Perform Key Agreement):" << getResponseErrorString(returnCode, response);
		return QByteArray();
	}
	return response.getEphemeralPublicKey();
}


QByteArray KeyAgreement::transmitGAMappingData(const QByteArray& pMappingData)
{
	// sende den PublicKey (D.3.4.)
	GABuilder commandBuilder(CommandApdu::CLA_COMMAND_CHAINING);
	commandBuilder.setPaceMappingData(pMappingData);
	GAMapNonceResponse response;

	ReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), response);
	if (returnCode != ReturnCode::OK || response.getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Mapping Data):" << getResponseErrorString(returnCode, response);
		return QByteArray();
	}
	return response.getMappingData();
}


QSharedPointer<GAMutualAuthenticationResponse> KeyAgreement::transmitGAMutualAuthentication(const QByteArray& pMutualAuthenticationData)
{
	GABuilder commandBuilder(CommandApdu::CLA);
	commandBuilder.setPaceAuthenticationToken(pMutualAuthenticationData);
	QSharedPointer<GAMutualAuthenticationResponse> response(new GAMutualAuthenticationResponse());

	ReturnCode returnCode = mCardConnectionWorker->transmit(commandBuilder.build(), *response);
	if (returnCode != ReturnCode::OK || response->getReturnCode() != StatusCode::SUCCESS)
	{
		qCCritical(card) << "Error on GA(Mutual Authentication):" << getResponseErrorString(returnCode, *response);
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
