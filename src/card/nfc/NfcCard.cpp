/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "NfcCard.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_nfc)


NfcCard::NfcCard(QNearFieldTarget* pNearFieldTarget)
	: Card()
	, mConnected(false)
	, mIsValid(true)
	, mNearFieldTarget(pNearFieldTarget)
{
	qCDebug(card_nfc) << "Card created";

	pNearFieldTarget->setParent(nullptr);
	connect(pNearFieldTarget, &QNearFieldTarget::error, this, &NfcCard::fireTargetError);
}


bool NfcCard::isValid() const
{
	return mIsValid;
}


bool NfcCard::invalidateTarget(const QNearFieldTarget* pNearFieldTarget)
{
	if (pNearFieldTarget == mNearFieldTarget.data())
	{
		mIsValid = false;
		return true;
	}

	return false;
}


CardReturnCode NfcCard::establishConnection()
{
	if (isConnected())
	{
		qCCritical(card_nfc) << "Card is already connected";
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	return CardReturnCode::OK;
}


CardReturnCode NfcCard::releaseConnection()
{
	if (!mIsValid || mNearFieldTarget == nullptr)
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


bool NfcCard::isConnected() const
{
	return mConnected;
}


void NfcCard::setProgressMessage(const QString& pMessage, int pProgress)
{
	QString message = generateProgressMessage(pMessage, pProgress);
	Q_EMIT fireSetProgressMessage(message);
}


void NfcCard::setErrorMessage(const QString& pMessage)
{
	QString message = generateErrorMessage(pMessage);
	Q_EMIT fireSetProgressMessage(message);
}


ResponseApduResult NfcCard::transmit(const CommandApdu& pCmd)
{
	if (!mIsValid || mNearFieldTarget == nullptr)
	{
		qCWarning(card_nfc) << "NearFieldTarget is no longer valid";
		return {CardReturnCode::COMMAND_FAILED};
	}

	qCDebug(card_nfc) << "Transmit command APDU:" << pCmd;

	if (!mNearFieldTarget->accessMethods().testFlag(QNearFieldTarget::AccessMethod::TagTypeSpecificAccess))
	{
		qCWarning(card_nfc) << "No TagTypeSpecificAccess supported";
		return {CardReturnCode::COMMAND_FAILED};
	}

	QNearFieldTarget::RequestId id = mNearFieldTarget->sendCommand(pCmd);
	if (!id.isValid())
	{
		qCWarning(card_nfc) << "Cannot write messages";
		return {CardReturnCode::COMMAND_FAILED};
	}

	if (!mNearFieldTarget->waitForRequestCompleted(id, 3000))
	{
		qCWarning(card_nfc) << "Transmit timeout reached";
		return {CardReturnCode::COMMAND_FAILED};
	}

	QVariant response = mNearFieldTarget->requestResponse(id);
	if (!response.isValid())
	{
		qCWarning(card_nfc) << "Invalid response received";
		return {CardReturnCode::COMMAND_FAILED};
	}

	const QByteArray recvBuffer = response.toByteArray();
	const ResponseApdu responseApdu(recvBuffer);
	qCDebug(card_nfc) << "Transmit response APDU:" << responseApdu;
	return {CardReturnCode::OK, responseApdu};
}
