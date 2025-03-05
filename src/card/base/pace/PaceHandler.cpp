/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "pace/PaceHandler.h"

#include "SecurityProtocol.h"
#include "apdu/CommandApdu.h"
#include "apdu/PacePinStatus.h"
#include "asn1/ASN1Struct.h"
#include "asn1/PaceInfo.h"
#include "pace/KeyAgreement.h"

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


SecurityProtocol PaceHandler::getPaceProtocol() const
{
	if (!mPaceInfo)
	{
		return SecurityProtocol(Oid(nullptr));
	}
	return mPaceInfo->getProtocol();
}


CardReturnCode PaceHandler::establishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pPassword)
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

		case CardReturnCode::RESPONSE_EMPTY:
			return CardReturnCode::RESPONSE_EMPTY;

		default:
			break;
	}

	KeyAgreementStatus keyAgreementStatus = mKeyAgreement->perform(pPassword);
	switch (keyAgreementStatus)
	{
		case KeyAgreementStatus::RETRY_ALLOWED:
			return CardReturnCode::RESPONSE_EMPTY;

		case KeyAgreementStatus::PROTOCOL_ERROR:
			return CardReturnCode::PROTOCOL_ERROR;

		case KeyAgreementStatus::COMMUNICATION_ERROR:
			return CardReturnCode::COMMAND_FAILED;

		case KeyAgreementStatus::FAILED:
			return CardReturnCode::INVALID_PASSWORD;

		case KeyAgreementStatus::SUCCESS:
			mEncryptionKey = mKeyAgreement->getEncryptionKey();
			mMacKey = mKeyAgreement->getMacKey();
			mCarCurr = mKeyAgreement->getCarCurr();
			mCarPrev = mKeyAgreement->getCarPrev();
			mIdIcc = mKeyAgreement->getCompressedCardPublicKey();
			qCDebug(card) << "Pace channel established";
			return CardReturnCode::OK;
	}

	Q_UNREACHABLE();
}


bool PaceHandler::initialize(const QSharedPointer<const EFCardAccess>& pEfCardAccess)
{
	if (!pEfCardAccess)
	{
		return false;
	}

	const auto& infos = pEfCardAccess->getPaceInfos();
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


bool PaceHandler::isSupportedProtocol(const QSharedPointer<const PaceInfo>& pPaceInfo) const
{
	if (pPaceInfo->getVersion() != 2)
	{
		qCWarning(card) << "Unsupported pace version:" << pPaceInfo->getVersion();
		return false;
	}

	const auto protocol = pPaceInfo->getProtocol();
	if (protocol.getKeyAgreement() == KeyAgreementType::ECDH &&
			protocol.getMapping() == MappingType::GM &&
			protocol.getKeySize() >= 16 &&
			pPaceInfo->isStandardizedDomainParameters())
	{
		qCDebug(card) << "Use ECDH with standardized domain parameters:" << pPaceInfo->getProtocol();
		return true;
	}

	qCWarning(card) << "Unsupported domain parameters:" << protocol;
	return false;
}


CardReturnCode PaceHandler::transmitMSESetAT(PacePasswordId pPasswordId)
{
	ASN1Struct cmdData;
	cmdData.append(ASN1Struct::CRYPTOGRAPHIC_MECHANISM_REFERENCE, mPaceInfo->getOid());
	cmdData.append(ASN1Struct::PASSWORD_REFERENCE, pPasswordId);
	cmdData.append(ASN1Struct::PRIVATE_KEY_REFERENCE, mPaceInfo->getParameterId());
	if (!mChat.isNull())
	{
		cmdData.append(mChat);
	}
	CommandApdu cmdApdu(Ins::MSE_SET, CommandApdu::PACE, CommandApdu::AUTHENTICATION_TEMPLATE, cmdData);

	const auto& [cardReturnCode, response] = mCardConnectionWorker->transmit(cmdApdu);
	mStatusMseSetAt = response.getStatusBytes();

	if (cardReturnCode != CardReturnCode::OK)
	{
		qCCritical(card) << "Error on MSE:Set AT";
		return response.isEmpty() ? CardReturnCode::RESPONSE_EMPTY : CardReturnCode::PROTOCOL_ERROR;
	}
	if (PacePinStatus::getRetryCounter(response.getStatusCode()) < 1)
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
