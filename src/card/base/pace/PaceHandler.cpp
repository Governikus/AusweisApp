/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "pace/PaceHandler.h"

#include "asn1/KnownOIDs.h"
#include "asn1/PACEInfo.h"
#include "Commands.h"
#include "FileRef.h"
#include "pace/ec/EllipticCurveFactory.h"
#include "pace/KeyAgreement.h"
#include "PersoSimWorkaround.h"

#include <exception>
#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

PaceHandler::PaceHandler(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: mCardConnectionWorker(pCardConnectionWorker)
	, mKeyAgreement()
	, mPaceInfo()
	, mStatusMseSetAt()
	, mIdIcc()
	, mEncryptionKey()
	, mMacKey()
	, mChat()
	, mCarCurr()
	, mCarPrev()
{
}


QByteArray PaceHandler::getPaceProtocol() const
{
	if (!mPaceInfo)
	{
		return QByteArray();
	}
	return mPaceInfo->getProtocol();
}


CardReturnCode PaceHandler::establishPaceChannel(PACE_PASSWORD_ID pPasswordId, const QString& pPassword)
{
	auto efCardAccess = mCardConnectionWorker->getReaderInfo().getCardInfo().getEfCardAccess();
	if (!initialize(efCardAccess))
	{
		return CardReturnCode::PROTOCOL_ERROR;
	}

	switch (transmitMSESetAT(pPasswordId))
	{
		case CardReturnCode::PROTOCOL_ERROR:
			return CardReturnCode::PROTOCOL_ERROR;

		case CardReturnCode::COMMAND_FAILED:
			return CardReturnCode::COMMAND_FAILED;

		default:
			;
	}

	KeyAgreementStatus keyAgreementStatus = mKeyAgreement->perform(pPassword);
	if (keyAgreementStatus == KeyAgreementStatus::PROTOCOL_ERROR)
	{
		return CardReturnCode::PROTOCOL_ERROR;
	}
	else if (keyAgreementStatus == KeyAgreementStatus::FAILED)
	{
		switch (pPasswordId)
		{
			case PACE_PASSWORD_ID::PACE_MRZ:
			// No separate error code (yet).
			case PACE_PASSWORD_ID::PACE_CAN:
				return CardReturnCode::INVALID_CAN;

			case PACE_PASSWORD_ID::PACE_PIN:
				return CardReturnCode::INVALID_PIN;

			case PACE_PASSWORD_ID::PACE_PUK:
				return CardReturnCode::INVALID_PUK;
		}
	}
	mEncryptionKey = mKeyAgreement->getEncryptionKey();
	mMacKey = mKeyAgreement->getMacKey();
	mCarCurr = mKeyAgreement->getCarCurr();
	mCarPrev = mKeyAgreement->getCarPrev();
	mIdIcc = mKeyAgreement->getCompressedCardPublicKey();
	qCDebug(card) << "Pace channel established";
	return CardReturnCode::OK;
}


bool PaceHandler::initialize(const QSharedPointer<const EFCardAccess>& pEfCardAccess)
{
	if (!pEfCardAccess)
	{
		return false;
	}

	const auto& infos = pEfCardAccess->getPACEInfos();
	for (const auto& paceInfo : infos)
	{
		if (isSupportedProtocol(paceInfo))
		{
			mPaceInfo = paceInfo;
			mKeyAgreement = KeyAgreement::create(mPaceInfo, mCardConnectionWorker);
			break;
		}
	}

	if (!mKeyAgreement)
	{
		qCCritical(card) << "No supported domain parameters found";
		return false;
	}
	return true;
}


bool PaceHandler::isSupportedProtocol(const QSharedPointer<const PACEInfo>& pPaceInfo) const
{
	if (pPaceInfo->getVersion() != 2)
	{
		qCWarning(card) << "Unsupported pace version:" << pPaceInfo->getVersion();
		return false;
	}

	const auto protocol = pPaceInfo->getProtocol();

	if (protocol == KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128 ||
			protocol == KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_192 ||
			protocol == KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_256)
	{
		if (pPaceInfo->isStandardizedDomainParameters())
		{
			qCDebug(card) << "Use ECDH with standardized domain parameters: " << pPaceInfo->getProtocol();
			return true;
		}
	}

	qCWarning(card) << "Unsupported domain parameters: " << pPaceInfo->getProtocol();
	return false;
}


CardReturnCode PaceHandler::transmitMSESetAT(PACE_PASSWORD_ID pPasswordId)
{
	CardReturnCode cardReturnCode = PersoSimWorkaround::sendingMseSetAt(mCardConnectionWorker);
	if (cardReturnCode != CardReturnCode::OK)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return CardReturnCode::COMMAND_FAILED;
	}

	MSEBuilder mseBuilder(MSEBuilder::P1::PERFORM_SECURITY_OPERATION, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(mPaceInfo->getProtocolValueBytes());
	mseBuilder.setPublicKey(pPasswordId);
	mseBuilder.setPrivateKey(mPaceInfo->getParameterId());
	if (!mChat.isNull())
	{
		mseBuilder.setChat(mChat);
	}

	ResponseApdu response;
	cardReturnCode = mCardConnectionWorker->transmit(mseBuilder.build(), response);

	Q_ASSERT(response.getBuffer().length() == 2);
	mStatusMseSetAt = response.getBuffer().left(2);

	const StatusCode responseReturnCode = response.getReturnCode();
	if (cardReturnCode != CardReturnCode::OK)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return responseReturnCode == StatusCode::EMPTY ? CardReturnCode::COMMAND_FAILED : CardReturnCode::PROTOCOL_ERROR;
	}
	if (responseReturnCode != StatusCode::SUCCESS && responseReturnCode != StatusCode::PIN_RETRY_COUNT_2 && responseReturnCode != StatusCode::PIN_SUSPENDED)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return CardReturnCode::PROTOCOL_ERROR;
	}
	return CardReturnCode::OK;
}


void PaceHandler::setChat(const QByteArray& pChat)
{
	mChat = pChat;
}


const QByteArray& PaceHandler::getEncryptionKey() const
{
	return mEncryptionKey;
}


const QByteArray& PaceHandler::getMacKey() const
{
	return mMacKey;
}


const QByteArray& PaceHandler::getCarCurr() const
{
	return mCarCurr;
}


const QByteArray& PaceHandler::getCarPrev() const
{
	return mCarPrev;
}


const QByteArray& PaceHandler::getIdIcc() const
{
	return mIdIcc;
}


const QByteArray& PaceHandler::getStatusMseSetAt() const
{
	return mStatusMseSetAt;
}
