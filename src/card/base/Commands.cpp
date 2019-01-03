/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Commands.h"

#include "asn1/ASN1Util.h"
#include "FileRef.h"
#include "SecureMessagingResponse.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


/*
 * The base class CommandApduBuilder
 */
CommandApduBuilder::CommandApduBuilder()
{
}


CommandApduBuilder::~CommandApduBuilder()
{
}


/*
 * SelectBuilder
 */
SelectBuilder::SelectBuilder(const FileRef& pFileRef)
	: CommandApduBuilder()
	, mFileRef(pFileRef)
{
}


CommandApdu SelectBuilder::build()
{
	static const char INS = char(0xA4);
	return CommandApdu(CommandApdu::CLA, INS, mFileRef.type, static_cast<char>(P2::NONE), mFileRef.path);
}


/*
 * GetChallengeBuilder
 */

GetChallengeBuilder::GetChallengeBuilder()
{
}


CommandApdu GetChallengeBuilder::build()
{
	static const char INS = char(0x84);
	return CommandApdu(CommandApdu::CLA, INS, 0, 0, QByteArray(), 0x08);
}


/*
 * GetChallengeResponse
 */

GetChallengeResponse::GetChallengeResponse()
	: ResponseApdu()
{
}


GetChallengeResponse::~GetChallengeResponse()
{
}


QByteArray GetChallengeResponse::getChallenge() const
{
	if (getDataLength() != 8)
	{
		qCCritical(card) << "Challenge has wrong size. Expect 8 bytes, got " << getDataLength();
	}
	return getData();
}


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


/*
 * PSOBuilder
 */
PSOBuilder::PSOBuilder(P1 p1, P2 p2)
	: CommandApduBuilder()
	, mP1(p1)
	, mP2(p2)
	, mCertificateBody()
	, mSignature()
{
}


void PSOBuilder::setCertificateBody(const QByteArray& pData)
{
	mCertificateBody = pData;
}


void PSOBuilder::setSignature(const QByteArray& pData)
{
	mSignature = pData;
}


CommandApdu PSOBuilder::build()
{
	static const int INS = 0x2a;

	QByteArray data;
	data += mCertificateBody;
	data += mSignature;

	return CommandApdu(CommandApdu::CLA, INS, char(mP1), char(mP2), data);
}


/*
 *  EABuilder
 */
EABuilder::EABuilder()
	: CommandApduBuilder()
	, mSignature()
{
}


void EABuilder::setSignature(const QByteArray& pData)
{
	mSignature = pData;
}


CommandApdu EABuilder::build()
{
	static const char INS = char(0x82);
	return CommandApdu(CommandApdu::CLA, INS, 0, 0, mSignature);
}


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


ReadBinaryBuilder::ReadBinaryBuilder(uint pOffset, int pLe)
	: CommandApduBuilder()
	, mOffset(pOffset)
	, mLe(pLe)
{
}


CommandApdu ReadBinaryBuilder::build()
{
	static const char INS = char(0xB0);
	return CommandApdu(CommandApdu::CLA, INS, static_cast<char>((mOffset & 0xff00) >> 8), static_cast<char>(mOffset & 0xff), QByteArray(), mLe);
}


ResetRetryCounterBuilder::ResetRetryCounterBuilder(const QByteArray& pPin)
	: CommandApduBuilder()
	, mPin(pPin)
{
}


CommandApdu ResetRetryCounterBuilder::build()
{
	static const char INS = 0x2c;
	// P1: 2 (change), 3 (unblock)
	const char p1 = mPin.isNull() ? char(3) : char(2);
	// P2: 3 (PIN) (2 (CAN) -- not used)
	// data: new PIN, when changing
	return CommandApdu(CommandApdu::CLA, INS, p1, 3, mPin);
}
