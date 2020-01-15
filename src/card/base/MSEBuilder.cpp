/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "MSEBuilder.h"

#include "asn1/ASN1Util.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(card)

/*
 * MSEBuilder
 */
MSEBuilder::MSEBuilder(P1 p1, P2 p2)
	: CommandApduBuilder()
	, mP1(p1)
	, mP2(p2)
	, mAuxiliaryData()
	, mOid()
	, mPublicKey()
	, mPrivateKey()
	, mEphemeralPublicKey()
	, mChat()
{
}


void MSEBuilder::setAuxiliaryData(const QByteArray& pData)
{
	mAuxiliaryData = pData;
}


void MSEBuilder::setOid(const QByteArray& pData)
{
	static const char TAG_OID = char(0x80);
	mOid = Asn1Util::encode(TAG_OID, pData);
}


void MSEBuilder::setPublicKey(const QByteArray& pData)
{
	static const char TAG_PUBLIC_KEY = char(0x83);
	mPublicKey = Asn1Util::encode(TAG_PUBLIC_KEY, pData);
}


void MSEBuilder::setPublicKey(PacePasswordId pPasswordId)
{
	static const char TAG_PUBLIC_KEY = char(0x83);
	QByteArray data;
	data += Enum<PacePasswordId>::getValue(pPasswordId);
	mPublicKey = Asn1Util::encode(TAG_PUBLIC_KEY, data);
}


void MSEBuilder::setPrivateKey(const QByteArray& pData)
{
	static const char TAG_PRIVATE_KEY = char(0x84);
	mPrivateKey = Asn1Util::encode(TAG_PRIVATE_KEY, pData);
}


void MSEBuilder::setEphemeralPublicKey(const QByteArray& pData)
{
	static const char TAG_EPHEMERAL_PUBLIC_KEY = char(0x91);
	mEphemeralPublicKey = Asn1Util::encode(TAG_EPHEMERAL_PUBLIC_KEY, pData);
}


void MSEBuilder::setChat(const QByteArray& pData)
{
	mChat = pData;
}


CommandApdu MSEBuilder::build()
{
	QByteArray data;
	data += mOid;
	data += mPublicKey;
	data += mPrivateKey;
	data += mAuxiliaryData;
	data += mEphemeralPublicKey;
	data += mChat;

	return CommandApdu(CommandApdu::CLA, static_cast<char>(MSEBuilder::INS::MANAGE_SECURITY_ENVIRONMENT), static_cast<char>(mP1), static_cast<char>(mP2), data);
}
