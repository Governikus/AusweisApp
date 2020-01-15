/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannelParser.h"

#include "asn1/ASN1TemplateUtil.h"
#include "Apdu.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


EstablishPaceChannelParser::EstablishPaceChannelParser(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription, const QByteArray& pCommandData)
	: mPasswordId(pPasswordId)
	, mChat(pChat)
	, mCertificateDescription(pCertificateDescription)
	, mCommandData(pCommandData)
{
}


EstablishPaceChannelParser EstablishPaceChannelParser::fromCcid(const QByteArray& pInput)
{
	CommandApdu command(pInput);

	if (command.getCLA() != char(0xFF)
			|| command.getINS() != char(0x9A)
			|| command.getP1() != 0x04
			|| command.getP2() != 0x02)
	{
		qCDebug(card) << "Decapsulation of command failed. Unexpected header.";
		return EstablishPaceChannelParser();
	}

	QByteArray commandData = command.getData();
	auto channelInput = decodeObject<ESTABLISHPACECHANNELINPUT>(commandData);

	Q_ASSERT(channelInput);
	if (!channelInput)
	{
		qCDebug(card) << "Decapsulation of command failed. Bad command data.";
		return EstablishPaceChannelParser();
	}

	PacePasswordId passwordId = PacePasswordId::PACE_PIN;
	Q_ASSERT(channelInput->mPasswordID);
	if (channelInput->mPasswordID)
	{
		char asn1_char = static_cast<char>(ASN1_INTEGER_get(channelInput->mPasswordID));
		if (Enum<PacePasswordId>::isValue(asn1_char))
		{
			passwordId = PacePasswordId(asn1_char);
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
	// Chat and certificate description are only available in authentications via PIN mode or CAN allowed mode
	if (passwordId == PacePasswordId::PACE_PIN || passwordId == PacePasswordId::PACE_CAN)
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

	return EstablishPaceChannelParser(passwordId, chat, certificateDescription, commandData);
}


PacePasswordId EstablishPaceChannelParser::getPasswordId() const
{
	return mPasswordId;
}


const QByteArray& EstablishPaceChannelParser::getChat() const
{
	return mChat;
}


const QByteArray& EstablishPaceChannelParser::getCertificateDescription() const
{
	return mCertificateDescription;
}


const QByteArray& EstablishPaceChannelParser::getCommandData() const
{
	return mCommandData;
}
