/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "pace/PaceHandler.h"

#include "Commands.h"
#include "FileRef.h"
#include "PersoSimWorkaround.h"
#include "asn1/KnownOIDs.h"
#include "asn1/PACEInfo.h"
#include "pace/KeyAgreement.h"
#include "pace/ec/EllipticCurveFactory.h"

#include <QLoggingCategory>
#include <exception>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

PaceHandler::PaceHandler(const QSharedPointer<CardConnectionWorker>& pCardConnectionWorker)
	: mCardConnectionWorker(pCardConnectionWorker)
	, mKeyAgreement()
	, mPaceInfo()
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


ReturnCode PaceHandler::establishPaceChannel(PACE_PIN_ID pPinId, const QString& pPin)
{
	auto efCardAccess = mCardConnectionWorker->getReaderInfo().getCardInfo().getEfCardAccess();
	if (!efCardAccess)
	{
		return ReturnCode::PROTOCOL_ERROR;
	}
	if (!initialize(efCardAccess))
	{
		return ReturnCode::PROTOCOL_ERROR;
	}
	if (!transmitMSESetAT(pPinId))
	{
		return ReturnCode::PROTOCOL_ERROR;
	}

	KeyAgreementStatus keyAgreementStatus = mKeyAgreement->perform(pPin);
	if (keyAgreementStatus == KeyAgreementStatus::PROTOCOLL_ERROR)
	{
		return ReturnCode::PROTOCOL_ERROR;
	}
	else if (keyAgreementStatus == KeyAgreementStatus::FAILED)
	{
		switch (pPinId)
		{
			case PACE_PIN_ID::PACE_MRZ:
			// No separate error code (yet).
			case PACE_PIN_ID::PACE_CAN:
				return ReturnCode::CAN_INVALID;

			case PACE_PIN_ID::PACE_PIN:
				return ReturnCode::PIN_INVALID;

			case PACE_PIN_ID::PACE_PUK:
				return ReturnCode::PUK_INVALID;
		}
	}
	mEncryptionKey = mKeyAgreement->getEncryptionKey();
	mMacKey = mKeyAgreement->getMacKey();
	mCarCurr = mKeyAgreement->getCarCurr();
	mCarPrev = mKeyAgreement->getCarPrev();
	mIdIcc = mKeyAgreement->getCompressedCardPublicKey();
	qCDebug(card) << "Pace channel established";
	return ReturnCode::OK;
}


bool PaceHandler::initialize(const QSharedPointer<const EFCardAccess>& pEfCardAccess)
{
	if (!pEfCardAccess)
	{
		return false;
	}

	const auto& infos = pEfCardAccess->getSecurityInfos<PACEInfo>();
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


bool PaceHandler::isSupportedProtocol(const QSharedPointer<PACEInfo>& pPaceInfo) const
{
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


bool PaceHandler::transmitMSESetAT(PACE_PIN_ID pPinId)
{
	PersoSimWorkaround::sendingMseSetAt(mCardConnectionWorker);

	MSEBuilder mseBuilder(MSEBuilder::P1::PERFORM_SECURITY_OPERATION, MSEBuilder::P2::SET_AT);
	mseBuilder.setOid(mPaceInfo->getProtocolValueBytes());
	mseBuilder.setPublicKey(pPinId);
	mseBuilder.setPrivateKey(mPaceInfo->getParameterId());
	if (!mChat.isNull())
	{
		mseBuilder.setChat(mChat, true);
	}

	ResponseApdu response;
	ReturnCode returnCode = mCardConnectionWorker->transmit(mseBuilder.build(), response);
	if (returnCode != ReturnCode::OK)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return false;
	}
	if (response.getReturnCode() != StatusCode::SUCCESS && response.getReturnCode() != StatusCode::PIN_RETRY_COUNT_2 && response.getReturnCode() != StatusCode::PIN_SUSPENDED)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return false;
	}
	return true;
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
