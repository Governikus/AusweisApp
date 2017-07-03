/*!
 * Commands.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Commands.h"

#include "FileRef.h"
#include "SecureMessagingResponse.h"
#include "asn1/ASN1Util.h"

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


void MSEBuilder::setPublicKey(PACE_PIN_ID pPin)
{
	static const char TAG_PUBLIC_KEY = char(0x83);
	QByteArray data;
	data += Enum<PACE_PIN_ID>::getValue(pPin);
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
	static const char INS = 0x22;

	QByteArray data;
	data += mOid;
	data += mPublicKey;
	data += mPrivateKey;
	data += mAuxiliaryData;
	data += mEphemeralPublicKey;
	data += mChat;

	return CommandApdu(CommandApdu::CLA, INS, static_cast<char>(mP1), static_cast<char>(mP2), data);
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

	return CommandApdu(mClassByte, INS, 0, 0, data, Apdu::SHORT_MAX_LE);
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
	char p1 = mPin.isNull() ? 3 : 2;
	// P2: 3 (PIN) (2 (CAN) -- not used)
	// data: new PIN, when changing
	return CommandApdu(CommandApdu::CLA, INS, p1, 3, mPin);
}


QByteArray PinModifyBuilder::createChangeEidPinCommandData(quint8 pTimeoutSeconds) const
{
	// According to ISO-7816-4, 7.5.10 RESET RETRY COUNTER command
	QByteArray abData = QByteArrayLiteral("002C0203");

	return createCommandData(pTimeoutSeconds, 0x00, 0x01, 0x02, QByteArray::fromHex(abData));
}


QByteArray PinModifyBuilder::createCommandData(quint8 pTimeoutSeconds, char pMsgIndex1, char pMsgIndex2, char pMsgIndex3, const QByteArray& pAbData) const
{
	// as defined in PC/SC, Part 10 "IFDs with Secure PIN Entry Capabilities"
	QByteArray command;
	//	bTimeOut (timeout in seconds)
	command += static_cast<char>(pTimeoutSeconds);
	//	bTimeOut2 (timeout in seconds after first key pressed)
	command += static_cast<char>(pTimeoutSeconds);
	//	bmFormatString (PIN format): system unit is bytes (0x80), ASCII format (0x02)
	command += char(0x82);
	//	bmPINBlockString (PIN block size and length info): PIN not in APDU command
	command += char(0x00);
	//	bmPINLengthFormat (format of PIN length field in APDU command): PIN not in APDU command
	command += char(0x00);
	//	bInsertionOffsetOld (insertion position offset for old PIN)
	command += char(0x00);
	//	bInsertionOffsetNew BYTE (insertion position offset for new PIN)
	command += char(0x00);
	//	wPINMaxExtraDigit USHORT (0xXXYY, min (XX) and max (length) of new PIN)
	command += 0x06;
	command += 0x06;
	//	bConfirmPIN (PIN confirmation options): confirm new PIN (0x01)
	command += 0x01;
	//	bEntryValidationCondition (new PIN validation options): validation key pressed (0x02)
	command += 0x02;
	//	bNumberMessage (number of display messages to be sent)
	command += 0x02;
	//	wLangId (language ID for display messages): German (0x0407)
	command += 0x07;
	command += 0x04;
	//	bMsgIndex1 (index (into reader table) of first message to display)
	command += pMsgIndex1;
	//	bMsgIndex2 (index (into reader table) of second message to display)
	command += pMsgIndex2;
	//	bMsgIndex3 (index (into reader table) of third message to display)
	command += pMsgIndex3;
	//	bTeoPrologue (T1 only: I-block prologue field to use): fill with 0
	command += char(0x00);
	command += char(0x00);
	command += char(0x00);

	if (pAbData.size() > 0xFF)
	{
		qCCritical(card) << "abData size bigger than 0xFF currently not supported.";
		Q_ASSERT(pAbData.size() <= 0xFF);
		return QByteArray();
	}
	//	ulDataLength (length of the APDU to be sent to ICC)
	command += static_cast<char>(pAbData.size());
	command += char(0x00);
	command += char(0x00);
	command += char(0x00);
	command += pAbData;

	return command;
}


CommandApdu PinModifyBuilder::createCommandDataCcid(quint8 pTimeoutSeconds) const
{
	// According to TR-03119 the command data has to be the full PC_to_RDR_Secure structure
	// According to Reiner SCT the firmware is implemented in such a way, that the command
	// data is expected as abPINOperationDataStucture

	// according to DWG_Smart-Card_CCID_Rev110.pdf
	QByteArray abPINDataStructure;

	//	bTimeOut (timeout in seconds)
	abPINDataStructure += static_cast<char>(pTimeoutSeconds);
	//	bmFormatString (PIN format): system unit is bytes (0x80), ASCII format (0x02)
	abPINDataStructure += char(0x82);
	//	bmPINBlockString (PIN block size and length info): PIN not in APDU command
	abPINDataStructure += char(0x00);
	//	bmPINLengthFormat (format of PIN length field in APDU command): PIN not in APDU command
	abPINDataStructure += char(0x00);
	//	bInsertionOffsetOld (insertion position offset for old PIN)
	abPINDataStructure += char(0x00);
	//	bInsertionOffsetNew BYTE (insertion position offset for new PIN)
	abPINDataStructure += char(0x00);
	//	wPINMaxExtraDigit USHORT (0xXXYY, min (XX) and max (length) of new PIN)
	abPINDataStructure += char(0x06);
	abPINDataStructure += char(0x06);
	//	bConfirmPIN (PIN confirmation options): confirm new PIN (0x01)
	abPINDataStructure += char(0x01);
	//	bEntryValidationCondition (new PIN validation options): validation key pressed (0x02)
	abPINDataStructure += char(0x02);
	//	bNumberMessage (number of display messages to be sent)
	abPINDataStructure += char(0x02);
	//	wLangId (language ID for display messages): German (0x0407)
	abPINDataStructure += char(0x07);
	abPINDataStructure += char(0x04);
	//	bMsgIndex1 (index (into reader table) of first message to display)
	abPINDataStructure += char(0x01);
	//	bMsgIndex2 (index (into reader table) of second message to display)
	abPINDataStructure += char(0x02);
	//	bMsgIndex3 (index (into reader table) of third message to display)
	abPINDataStructure += char(0x00);
	//	bTeoPrologue (T1 only: I-block prologue field to use): fill with 0
	abPINDataStructure += char(0x00);
	abPINDataStructure += char(0x00);
	abPINDataStructure += char(0x00);
	//	abData (APDU to be sent to ICC)
	abPINDataStructure += char(0x00); // CLA: command
	abPINDataStructure += char(0x2c); // INS: Reset Retry Counter
	abPINDataStructure += char(0x02); // P1: new PIN/CAN
	abPINDataStructure += char(0x03); // P2: PIN
	QByteArray abPINOperationDataStucture;
	abPINOperationDataStucture += char(0x01); //bPINOperation
	abPINOperationDataStucture += abPINDataStructure; //abPINDataStructure

	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x10, abPINOperationDataStucture);
}


void PinModifyOutput::parse(const QByteArray& pData)
{
	if (pData.size() != 2)
	{
		mReturnCode = CardReturnCode::UNKNOWN;
	}

	const int errorCode = static_cast<quint8>(pData.at(0)) << 8 | static_cast<quint8>(pData.at(1));
	switch (errorCode)
	{
		case 0x6400:
			// operation timed out
			mReturnCode = CardReturnCode::INPUT_TIME_OUT;
			break;

		case 0x6401:
			// operation canceled by "Cancel" button
			mReturnCode = CardReturnCode::CANCELLATION_BY_USER;
			break;

		case 0x6402:
			// the two new PIN entries don't match
			mReturnCode = CardReturnCode::NEW_PIN_MISMATCH;
			break;

		case 0x6403:
			// entered PIN too short/long
			mReturnCode = CardReturnCode::NEW_PIN_INVALID_LENGTH;
			break;

		case 0x6b80:
			// invalid parameter in passed structure
			mReturnCode = CardReturnCode::COMMAND_FAILED;
			break;

		case 0x6982:
			// terminal is not authorized to unblock or change the PIN
			mReturnCode = CardReturnCode::UNKNOWN;
			break;

		case 0x9000:
			// success
			mReturnCode = CardReturnCode::OK;
			break;

		default:
			qCDebug(card) << "unknown error:" << pData.toHex();
			mReturnCode = CardReturnCode::UNKNOWN;
			break;
	}
}


void PinModifyOutput::parseFromCcid(const QByteArray& pData)
{
	parse(pData);
}


CardReturnCode PinModifyOutput::getReturnCode() const
{
	return mReturnCode;
}
