/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPACEChannelParser.h"

#include "asn1/ASN1TemplateUtil.h"
#include "Apdu.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


EstablishPACEChannelParser::EstablishPACEChannelParser(PACE_PASSWORD_ID pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, const QByteArray& pCommandData)
	: mPasswordId(pPasswordId)
	, mChat(pChat)
	, mCertificateDescription(pCertificateDescription)
	, mCommandData(pCommandData)
{

}


EstablishPACEChannelParser EstablishPACEChannelParser::fromCcid(const QByteArray& pInput)
{
	CommandApdu command(pInput);

	if (command.getCLA() != char(0xFF)
			|| command.getINS() != char(0x9A)
			|| command.getP1() != 0x04
			|| command.getP2() != 0x02)
	{
		qCDebug(card) << "Decapsulation of command failed. Unexpected header.";
		return EstablishPACEChannelParser();
	}

	QByteArray commandData = command.getData();
	auto channelInput = decodeObject<ESTABLISHPACECHANNELINPUT>(commandData);

	Q_ASSERT(channelInput);
	if (!channelInput)
	{
		qCDebug(card) << "Decapsulation of command failed. Bad command data.";
		return EstablishPACEChannelParser();
	}

	PACE_PASSWORD_ID passwordId = PACE_PASSWORD_ID::PACE_PIN;
	Q_ASSERT(channelInput->mPasswordID);
	if (channelInput->mPasswordID)
	{
		char asn1_char = static_cast<char>(ASN1_INTEGER_get(channelInput->mPasswordID));
		if (Enum<PACE_PASSWORD_ID>::isValue(asn1_char))
		{
			passwordId = PACE_PASSWORD_ID(asn1_char);
		}
		else
		{
			qCDebug(card) << "Decapsulation: Bad PIN ID!";
			Q_ASSERT(false);
		}
	}
	else
	{
		qCDebug(card) << "Decapsulation: No PIN ID!";
		Q_ASSERT(false);
	}

	QByteArray chat;
	QByteArray certificateDescription;
	// Chat and certificate description are only available in PIN mode in an authentication.
	if (passwordId == PACE_PASSWORD_ID::PACE_PIN)
	{
		if (channelInput->mCertificateDescription)
		{
			certificateDescription = channelInput->mCertificateDescription->encode();
		}
		else
		{
			qCDebug(card) << "Decapsulation: No certificate description";
		}

		if (channelInput->mCHAT)
		{
			chat = Asn1OctetStringUtil::getValue(channelInput->mCHAT);
		}
		else
		{
			qCDebug(card) << "Decapsulation: No CHAT";
		}
	}

	return EstablishPACEChannelParser(passwordId, chat, certificateDescription, commandData);
}


PACE_PASSWORD_ID EstablishPACEChannelParser::getPasswordId() const
{
	return mPasswordId;
}


const QByteArray& EstablishPACEChannelParser::getChat() const
{
	return mChat;
}


const QByteArray& EstablishPACEChannelParser::getCertificateDescription() const
{
	return mCertificateDescription;
}


const QByteArray& EstablishPACEChannelParser::getCommandData() const
{
	return mCommandData;
}
