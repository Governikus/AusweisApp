/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "NfcBridge.h"
#include "NfcCard.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card_nfc)

NfcCard::NfcCard()
	: Card()
	, mConnected(false)
{
}


NfcCard::~NfcCard()
{
}


ReturnCode NfcCard::connect()
{
	if (isConnected())
	{
		qCCritical(card_nfc) << "Card is already connected";
		return ReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	return ReturnCode::OK;
}


ReturnCode NfcCard::disconnect()
{
	if (!isConnected())
	{
		qCCritical(card_nfc) << "Card is already disconnected";
		return ReturnCode::COMMAND_FAILED;
	}

	mConnected = false;
	return ReturnCode::OK;
}


bool NfcCard::isConnected()
{
	return mConnected;
}


ReturnCode NfcCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
#ifdef Q_OS_ANDROID
	qCDebug(card_nfc) << "Transmit command APDU: " << pCmd.getBuffer().toHex();
	QByteArray recvBuffer = NfcBridge::getInstance().sendData(pCmd.getBuffer());

	if (recvBuffer.isEmpty())
	{
		qCWarning(card_nfc) << "No response received";
		return ReturnCode::COMMAND_FAILED;
	}

	NfcConnectorCode status = static_cast<NfcConnectorCode>(recvBuffer.at(0));
	if (status == NfcConnectorCode::SUCCESS)
	{
		pRes.setBuffer(recvBuffer.mid(1));
		qCDebug(card_nfc) << "Transmit response APDU: " << pRes.getBuffer().toHex();
		return ReturnCode::OK;
	}
	else if (status == NfcConnectorCode::ERROR_TAG_LOST || status == NfcConnectorCode::ERROR_NO_TAG)
	{
		qCWarning(card_nfc) << "Transmit error: " << status;
		Q_EMIT fireCardRemoved();
		return ReturnCode::COMMAND_FAILED;
	}
	else
	{
		qCWarning(card_nfc) << "Transmit error: " << status;
		return ReturnCode::COMMAND_FAILED;
	}
#endif

	Q_UNUSED(pCmd);
	Q_UNUSED(pRes);
	return ReturnCode::UNDEFINED;
}


ReturnCode NfcCard::establishPaceChannel(PACE_PIN_ID pPinId, const QByteArray& pChat, const QByteArray& pCertificateDescription, EstablishPACEChannelOutput& pChannelOutput, int pTimeoutSeconds)
{
	Q_UNUSED(pPinId);
	Q_UNUSED(pChat);
	Q_UNUSED(pCertificateDescription);
	Q_UNUSED(pChannelOutput);
	Q_UNUSED(pTimeoutSeconds);
	qCWarning(card_nfc) << "NFC does not support establishPaceChannel";
	return ReturnCode::COMMAND_FAILED;
}


ReturnCode NfcCard::setEidPin(unsigned int pTimeoutSeconds)
{
	Q_UNUSED(pTimeoutSeconds);
	qCWarning(card_nfc) << "NFC does not support setEidPin";
	return ReturnCode::COMMAND_FAILED;
}
