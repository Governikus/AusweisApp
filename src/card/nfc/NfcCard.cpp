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
{
}


NfcCard::~NfcCard()
{
}


CardReturnCode NfcCard::connect()
{
#ifdef Q_OS_ANDROID
	if (NfcBridge::getInstance().connectCard())
	{
		return CardReturnCode::OK;
	}
#endif
	return CardReturnCode::COMMAND_FAILED;
}


CardReturnCode NfcCard::disconnect()
{
#ifdef Q_OS_ANDROID
	if (NfcBridge::getInstance().disconnectCard())
	{
		return CardReturnCode::OK;
	}
#endif
	return CardReturnCode::COMMAND_FAILED;
}


bool NfcCard::isConnected()
{
#ifdef Q_OS_ANDROID
	return NfcBridge::getInstance().isCardConnected();

#endif
	return false;
}


CardReturnCode NfcCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
#ifdef Q_OS_ANDROID
	qCDebug(card_nfc) << "Transmit command APDU: " << pCmd.getBuffer().toHex();
	QByteArray recvBuffer = NfcBridge::getInstance().sendData(pCmd.getBuffer());

	if (recvBuffer.isEmpty())
	{
		qCWarning(card_nfc) << "No response received";
		return CardReturnCode::COMMAND_FAILED;
	}

	NfcConnectorCode status = static_cast<NfcConnectorCode>(recvBuffer.at(0));
	if (status == NfcConnectorCode::SUCCESS)
	{
		pRes.setBuffer(recvBuffer.mid(1));
		qCDebug(card_nfc) << "Transmit response APDU: " << pRes.getBuffer().toHex();
		return CardReturnCode::OK;
	}
	else if (status == NfcConnectorCode::ERROR_TAG_LOST || status == NfcConnectorCode::ERROR_NO_TAG)
	{
		qCWarning(card_nfc) << "Transmit error: " << status;
		Q_EMIT fireCardRemoved();
		return CardReturnCode::COMMAND_FAILED;
	}
	else
	{
		qCWarning(card_nfc) << "Transmit error: " << status;
		return CardReturnCode::COMMAND_FAILED;
	}
#endif

	Q_UNUSED(pCmd);
	Q_UNUSED(pRes);
	return CardReturnCode::UNDEFINED;
}
