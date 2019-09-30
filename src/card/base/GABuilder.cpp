/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "GABuilder.h"

#include "asn1/ASN1Util.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)

/*
 *  GABuilder
 */
GABuilder::GABuilder(char pClassByte)
	: CommandApduBuilder()
	, mClassByte(pClassByte)
	, mCaEphemeralPublicKey()
	, mPaceMappingData()
	, mPaceEphemeralPublicKey()
	, mPaceAuthenticationToken()
{
}


void GABuilder::setCaEphemeralPublicKey(const QByteArray& pData)
{
	static const char TAG_EPHEMERAL_PUBLIC_KEY = char(0x80);
	mCaEphemeralPublicKey = Asn1Util::encode(TAG_EPHEMERAL_PUBLIC_KEY, pData);
}


void GABuilder::setPaceMappingData(const QByteArray& pData)
{
	static const char TAG_PACE_MAPPING_DATA = char(0x81);
	mPaceMappingData = Asn1Util::encode(TAG_PACE_MAPPING_DATA, pData);
}


void GABuilder::setPaceEphemeralPublicKey(const QByteArray& pData)
{
	static const char TAG_PACE_EPHEMERAL_PUBLIC_KEY = char(0x83);
	mPaceEphemeralPublicKey = Asn1Util::encode(TAG_PACE_EPHEMERAL_PUBLIC_KEY, pData);
}


void GABuilder::setPaceAuthenticationToken(const QByteArray& pData)
{
	static const char TAG_PACE_AUTHENTICATION_TOKEN = char(0x85);
	mPaceAuthenticationToken = Asn1Util::encode(TAG_PACE_AUTHENTICATION_TOKEN, pData);
}


CommandApdu GABuilder::build()
{
	static const char INS = char(0x86);
	static const char TAG_DYNAMIC_AUTHENTICATION_DATA = 0x7C;

	QByteArray data;
	if (!mCaEphemeralPublicKey.isNull())
	{
		data += mCaEphemeralPublicKey;
	}
	else if (!mPaceMappingData.isNull())
	{
		data += mPaceMappingData;
	}
	else if (!mPaceEphemeralPublicKey.isNull())
	{
		data += mPaceEphemeralPublicKey;
	}
	else if (!mPaceAuthenticationToken.isNull())
	{
		data += mPaceAuthenticationToken;
	}
	data = Asn1Util::encode(TAG_DYNAMIC_AUTHENTICATION_DATA, data);

	return CommandApdu(mClassByte, INS, 0, 0, data, CommandApdu::SHORT_MAX_LE);
}
