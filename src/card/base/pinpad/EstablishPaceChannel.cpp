/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannel.h"

#include "LengthValue.h"
#include "apdu/CommandApdu.h"
#include "asn1/ASN1Util.h"

#include <QDataStream>
#include <QLoggingCategory>
#include <QtEndian>


// TR-03119 D.3 Mapping on CCID
#define CCID_ESTABLISH_PACE_CHANNEL QByteArray::fromHex("FF9A0402")


Q_DECLARE_LOGGING_CATEGORY(card)


using namespace governikus;


namespace governikus
{

/*
 * There is no NUMERICSTRING implementation available in the macro system of OpenSSL,
 * so we define it.
 */
ASN1_ITEM_TEMPLATE(NUMERICSTRING) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG | ASN1_TFLG_OPTIONAL, 0x12, NUMERICSTRING, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(NUMERICSTRING)

ASN1_SEQUENCE(ESTABLISHPACECHANNELINPUT) = {
	ASN1_EXP(ESTABLISHPACECHANNELINPUT, mPasswordID, ASN1_INTEGER, 0x01),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELINPUT, mTransmittedPassword, NUMERICSTRING, 0x02),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELINPUT, mCHAT, ASN1_OCTET_STRING, 0x03),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELINPUT, mCertificateDescription, CertificateDescription, 0x04),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELINPUT, mHashOID, ASN1_OBJECT, 0x05)
}


ASN1_SEQUENCE_END(ESTABLISHPACECHANNELINPUT)
IMPLEMENT_ASN1_FUNCTIONS(ESTABLISHPACECHANNELINPUT)
IMPLEMENT_ASN1_OBJECT(ESTABLISHPACECHANNELINPUT)


}  // namespace governikus


EstablishPaceChannel::EstablishPaceChannel(PacePasswordId pPasswordId, const QByteArray& pChat, const QByteArray& pCertificateDescription)
	: mPasswordId(pPasswordId)
	, mChat(pChat)
	, mCertificateDescription(pCertificateDescription)
	, mPassword()
{
}


void EstablishPaceChannel::setPassword(const QByteArray& pPassword)
{
	mPassword = pPassword;
}


bool EstablishPaceChannel::isCcid(const QByteArray& pInput)
{
	return pInput.startsWith(CCID_ESTABLISH_PACE_CHANNEL);
}


bool EstablishPaceChannel::fromCcid(const QByteArray& pInput)
{
	mPasswordId = PacePasswordId::UNKNOWN;
	mChat.clear();
	mCertificateDescription.clear();

	CommandApdu command(pInput);

	if (command.getHeaderBytes() != CCID_ESTABLISH_PACE_CHANNEL)
	{
		qCDebug(card) << "Decapsulation of command failed. Unexpected header.";
		return false;
	}

	QByteArray commandData = command.getData();
	auto channelInput = decodeObject<ESTABLISHPACECHANNELINPUT>(commandData);

	Q_ASSERT(channelInput);
	if (!channelInput)
	{
		qCDebug(card) << "Decapsulation of command failed. Bad command data.";
		return false;
	}

	const auto asn1_char = static_cast<char>(ASN1_INTEGER_get(channelInput->mPasswordID));
	if (!Enum<PacePasswordId>::isValue(asn1_char))
	{
		qCDebug(card) << "Decapsulation: Bad PIN ID!";
		return false;
	}

	mPasswordId = PacePasswordId(asn1_char);
	// Chat and certificate description are only available in authentications via PIN mode or CAN allowed mode
	if (mPasswordId == PacePasswordId::PACE_PIN || mPasswordId == PacePasswordId::PACE_CAN)
	{
		if (channelInput->mCertificateDescription)
		{
			mCertificateDescription = channelInput->mCertificateDescription->encode();
		}
		else
		{
			qCDebug(card) << "Decapsulation: No certificate description";
		}

		if (channelInput->mCHAT)
		{
			mChat = Asn1OctetStringUtil::getValue(channelInput->mCHAT);
		}
		else
		{
			qCDebug(card) << "Decapsulation: No CHAT";
		}
	}

	return true;
}


bool EstablishPaceChannel::fromInputData(const QByteArray& pInput)
{
	mPasswordId = PacePasswordId::UNKNOWN;
	mChat.clear();
	mCertificateDescription.clear();

	if (pInput.size() < 5)
	{
		qCDebug(card) << "Decapsulation of command failed. Wrong size.";
		return false;
	}

	quint8 passwordId = qFromLittleEndian<quint8>(pInput.data());
	if (Enum<PacePasswordId>::isValue(passwordId))
	{
		mPasswordId = PacePasswordId(passwordId);
	}
	else
	{
		qCDebug(card) << "Decapsulation: Bad PIN ID!";
		return false;
	}

	int it = 1;
	mChat = LengthValue::readByteArray<quint8>(pInput, it);
	// "If the PIN to be used is not secret (e.g. printed on the card/stored in the host), it may
	// be delivered by the host." In our case this is not relevant so we can ignore this field.
	LengthValue::readByteArray<quint8>(pInput, it).toHex();
	mCertificateDescription = LengthValue::readByteArray<quint16>(pInput, it);

	// Chat and certificate description are only available in authentications via PIN mode or CAN allowed mode
	if (mPasswordId == PacePasswordId::PACE_PIN || mPasswordId == PacePasswordId::PACE_CAN)
	{
		if (mChat.isEmpty())
		{
			qCDebug(card) << "Decapsulation: No CHAT";
		}
		if (mCertificateDescription.isEmpty())
		{
			qCDebug(card) << "Decapsulation: No certificate description";
		}
	}

	if (pInput.size() != it)
	{
		qCDebug(card) << "Decapsulation of command failed. Wrong size.";
		return false;
	}

	return true;
}


PacePasswordId EstablishPaceChannel::getPasswordId() const
{
	return mPasswordId;
}


const QByteArray& EstablishPaceChannel::getChat() const
{
	return mChat;
}


const QByteArray& EstablishPaceChannel::getCertificateDescription() const
{
	return mCertificateDescription;
}


QByteArray EstablishPaceChannel::createInputData() const
{
	QByteArray inputData;
	inputData += static_cast<char>(mPasswordId);

	if (mChat.size() > 0xFF)
	{
		qCCritical(card) << "Certificate Holder Authorization Template of size > 0xFF not supported";
		Q_ASSERT(mChat.size() <= 0xFF);
		return QByteArray();
	}
	inputData += static_cast<char>(mChat.size());
	inputData += mChat;

	inputData += static_cast<char>(mPassword.size());
	inputData += mPassword;

	if (mCertificateDescription.size() > 0xFFFF)
	{
		qCCritical(card) << "Certificate Description of size > 0xFFFF not supported";
		Q_ASSERT(mCertificateDescription.size() <= 0xFFFF);
		return QByteArray();
	}
	inputData += static_cast<char>((mCertificateDescription.size() >> 0) & 0xFF);
	inputData += static_cast<char>((mCertificateDescription.size() >> 8) & 0xFF);
	inputData += mCertificateDescription;

	return inputData;
}


QByteArray EstablishPaceChannel::createCommandData() const
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_ESTABLISH_PACE_CHANNEL = 0x02;

	QByteArray commandData;
	commandData += INDEX_ESTABLISH_PACE_CHANNEL;

	const auto& inputData = createInputData();
	if (inputData.size() > 0xFFFF)
	{
		qCCritical(card) << "InputData of size > 0xFFFF not supported";
		Q_ASSERT(inputData.size() <= 0xFFFF);
		return QByteArray();
	}
	commandData += static_cast<char>((inputData.size() >> 0) & 0xFF);
	commandData += static_cast<char>((inputData.size() >> 8) & 0xFF);
	commandData += inputData;

	return commandData;
}


QByteArray EstablishPaceChannel::createCommandDataCcid() const
{
	auto channelInput = newObject<ESTABLISHPACECHANNELINPUT>();

	ASN1_INTEGER_set(channelInput->mPasswordID, static_cast<long>(mPasswordId));

	if (!mPassword.isNull())
	{
		channelInput->mTransmittedPassword = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mPassword, channelInput->mTransmittedPassword);
	}

	if (!mChat.isNull())
	{
		channelInput->mCHAT = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mChat, channelInput->mCHAT);
	}
	if (!mCertificateDescription.isEmpty())
	{
		const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(mCertificateDescription.constData());
		decodeAsn1Object(&channelInput->mCertificateDescription, &unsignedCharPointer, static_cast<long>(mCertificateDescription.size()));
	}

	QByteArray data = encodeObject(channelInput.data());
	return CommandApdu(CCID_ESTABLISH_PACE_CHANNEL, data, CommandApdu::SHORT_MAX_LE);
}
