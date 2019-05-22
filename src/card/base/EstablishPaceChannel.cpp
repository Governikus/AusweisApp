/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannel.h"

#include "asn1/ASN1Util.h"
#include "ResponseApdu.h"

#include <QDataStream>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


namespace
{
template<typename T> QByteArray readByteArray(const QByteArray& pInput, int& pOffset)
{
	Q_ASSERT(sizeof(T) < INT_MAX);

	T length = qFromLittleEndian<T>(pInput.data() + pOffset);
	pOffset += static_cast<int>(sizeof(T));
	QByteArray result = pInput.mid(pOffset, length);
	pOffset += length;
	return result;
}


} // namespace


namespace governikus
{

/*
 * There is no NUMERICSTRING implementation available in the macro system of OpenSSL,
 * so we define it.
 */
ASN1_ITEM_TEMPLATE(NUMERICSTRING) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG, 0x12, NUMERICSTRING, ASN1_OCTET_STRING)
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


EstablishPaceChannel::EstablishPaceChannel()
	: mPasswordId(PacePasswordId::PACE_MRZ)
	, mChat(nullptr)
	, mCertificateDescription()
{
}


void EstablishPaceChannel::setCertificateDescription(const QByteArray& pCertificateDescription)
{
	mCertificateDescription = pCertificateDescription;
}


void EstablishPaceChannel::setChat(const QByteArray& pChat)
{
	mChat = pChat;
}


void EstablishPaceChannel::setPasswordId(PacePasswordId pPasswordId)
{
	mPasswordId = pPasswordId;
}


QByteArray EstablishPaceChannel::createCommandData()
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_ESTABLISH_PACE_CHANNEL = 0x02;

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

	inputData += '\0'; // length of PIN

	if (mCertificateDescription.size() > 0xFFFF)
	{
		qCCritical(card) << "Certificate Description of size > 0xFFFF not supported";
		Q_ASSERT(mCertificateDescription.size() <= 0xFFFF);
		return QByteArray();
	}
	inputData += static_cast<char>((mCertificateDescription.size() >> 0) & 0xff);
	inputData += static_cast<char>((mCertificateDescription.size() >> 8) & 0xff);
	inputData += mCertificateDescription;

	QByteArray commandData;
	commandData += (INDEX_ESTABLISH_PACE_CHANNEL);

	if (inputData.size() > 0xFFFF)
	{
		qCCritical(card) << "InputData of size > 0xFFFF not supported";
		Q_ASSERT(inputData.size() <= 0xFFFF);
		return QByteArray();
	}
	commandData += static_cast<char>((inputData.size() >> 0) & 0xff);
	commandData += static_cast<char>((inputData.size() >> 8) & 0xff);
	commandData += inputData;
	return commandData;
}


CommandApdu EstablishPaceChannel::createCommandDataCcid()
{
	auto channelInput = newObject<ESTABLISHPACECHANNELINPUT>();

	ASN1_INTEGER_set(channelInput->mPasswordID, static_cast<long>(mPasswordId));
	if (!mChat.isNull())
	{
		channelInput->mCHAT = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mChat, channelInput->mCHAT);
	}
	if (!mCertificateDescription.isEmpty())
	{
		const uchar* unsignedCharPointer = reinterpret_cast<const uchar*>(mCertificateDescription.constData());
		decodeAsn1Object(&channelInput->mCertificateDescription, &unsignedCharPointer, mCertificateDescription.size());
	}

	QByteArray data = encodeObject(channelInput.data());

	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x02, data, CommandApdu::SHORT_MAX_LE);
}
