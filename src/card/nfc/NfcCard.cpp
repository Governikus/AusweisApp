/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "NfcCard.h"

#include <QLoggingCategory>
#include <QNdefMessage>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


void NfcCard::onError(QNearFieldTarget::Error pError, const QNearFieldTarget::RequestId& pId)
{
	Q_UNUSED(pId);
	qCWarning(card_nfc) << "Error:" << pError;
}


NfcCard::NfcCard(QNearFieldTarget* pNearFieldTarget)
	: Card()
	, mConnected(false)
	, mIsValid(true)
	, mNearFieldTarget(pNearFieldTarget)
{
	qCDebug(card_nfc) << "Card created";

	QObject::connect(pNearFieldTarget, &QNearFieldTarget::error, this, &NfcCard::onError);

	pNearFieldTarget->setKeepConnection(true);
}


NfcCard::~NfcCard()
{
	//	Memory is managed by QNearFieldManager
	//	delete mNearFieldTarget;
}


bool NfcCard::isValid() const
{
	return mIsValid;
}


bool NfcCard::invalidateTarget(QNearFieldTarget* pNearFieldTarget)
{
	if (pNearFieldTarget == mNearFieldTarget)
	{
		mIsValid = false;
		return true;
	}

	return false;
}


CardReturnCode NfcCard::connect()
{
	if (isConnected())
	{
		qCCritical(card_nfc) << "Card is already connected";
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	return CardReturnCode::OK;
}


CardReturnCode NfcCard::disconnect()
{
	if (!mIsValid || !mNearFieldTarget)
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return CardReturnCode::COMMAND_FAILED;
	}

	if (!isConnected())
	{
		qCCritical(card_nfc) << "Card is already disconnected";
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = false;
	mNearFieldTarget->disconnect();
	return CardReturnCode::OK;
}


bool NfcCard::isConnected()
{
	return mConnected;
}


CardReturnCode NfcCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	if (!mIsValid || !mNearFieldTarget)
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return CardReturnCode::COMMAND_FAILED;
	}

	qCDebug(card_nfc) << "Transmit command APDU:" << pCmd.getBuffer().toHex();

	if (!mNearFieldTarget->accessMethods().testFlag(QNearFieldTarget::AccessMethod::TagTypeSpecificAccess))
	{
		qCWarning(card_nfc) << "No TagTypeSpecificAccess supported";
		return CardReturnCode::COMMAND_FAILED;
	}

	QNearFieldTarget::RequestId id = mNearFieldTarget->sendCommand(pCmd.getBuffer());
	if (!id.isValid())
	{
		qCWarning(card_nfc) << "Cannot write messages";
		return CardReturnCode::COMMAND_FAILED;
	}

	if (!mNearFieldTarget->waitForRequestCompleted(id, 1500))
	{
		qCWarning(card_nfc) << "Transmit timeout reached";
		return CardReturnCode::COMMAND_FAILED;
	}

	QVariant response = mNearFieldTarget->requestResponse(id);
	if (!response.isValid())
	{
		qCWarning(card_nfc) << "Invalid response received";
		return CardReturnCode::COMMAND_FAILED;
	}

	QByteArray recvBuffer = response.toByteArray();
	qCDebug(card_nfc) << "Transmit response APDU:" << recvBuffer.toHex();
	pRes.setBuffer(recvBuffer);
	return CardReturnCode::OK;
}
