/*!
 * Commands.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "Commands.h"
#include "FileRef.h"

#include <QLoggingCategory>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)

namespace
{

QByteArray buildAsn1Structure(char pTag, const QByteArray& pData, bool pAlreadyAsn1Encoded = false)
{
	if (pAlreadyAsn1Encoded)
	{
		return pData;
	}
	QByteArray result;
	result.append(pTag);
	result.append(static_cast<char>(pData.size()));
	result.append(pData);
	return result;
}


QByteArray buildAsn1Structure(const char* const pTag, const QByteArray& pData, bool pAlreadyAsn1Encoded = false)
{
	if (pAlreadyAsn1Encoded)
	{
		return pData;
	}
	QByteArray result;
	result.append(pTag);
	result.append(static_cast<char>(pData.size()));
	result.append(pData);
	return result;
}


}


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


void MSEBuilder::setAuxiliaryData(const QByteArray& pData, bool pAlreadyAsn1Encoded)
{
	static const char TAG_AUXILIARY_DATA = 0x67;
	mAuxiliaryData = buildAsn1Structure(TAG_AUXILIARY_DATA, pData, pAlreadyAsn1Encoded);
}


void MSEBuilder::setOid(const QByteArray& pData)
{
	static const char TAG_OID = char(0x80);
	mOid = buildAsn1Structure(TAG_OID, pData);
}


void MSEBuilder::setPublicKey(const QByteArray& pData)
{
	static const char TAG_PUBLIC_KEY = char(0x83);
	mPublicKey = buildAsn1Structure(TAG_PUBLIC_KEY, pData);
}


void MSEBuilder::setPublicKey(PACE_PIN_ID pPin)
{
	static const char TAG_PUBLIC_KEY = char(0x83);
	QByteArray data;
	data.append(static_cast<char>(pPin));
	mPublicKey = buildAsn1Structure(TAG_PUBLIC_KEY, data);
}


void MSEBuilder::setPrivateKey(const QByteArray& pData)
{
	static const char TAG_PRIVATE_KEY = char(0x84);
	mPrivateKey = buildAsn1Structure(TAG_PRIVATE_KEY, pData);
}


void MSEBuilder::setEphemeralPublicKey(const QByteArray& pData)
{
	static const char TAG_EPHEMERAL_PUBLIC_KEY = char(0x91);
	mEphemeralPublicKey = buildAsn1Structure(TAG_EPHEMERAL_PUBLIC_KEY, pData);
}


void MSEBuilder::setChat(const QByteArray& pData, bool pAlreadyAsn1Encoded)
{
	static const char* const TAG_CHAT = "\x7F\x4C";
	mChat = buildAsn1Structure(TAG_CHAT, pData, pAlreadyAsn1Encoded);
}


CommandApdu MSEBuilder::build()
{
	static const char INS = 0x22;

	QByteArray data;
	data.append(mOid);
	data.append(mPublicKey);
	data.append(mPrivateKey);
	data.append(mAuxiliaryData);
	data.append(mEphemeralPublicKey);
	data.append(mChat);

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


void PSOBuilder::setCertificateBody(const QByteArray& pData, bool pAlreadyAsn1Encoded)
{
	static const char* const TAG_CERT_BODY = "\x7F\x4E";
	mCertificateBody = buildAsn1Structure(TAG_CERT_BODY, pData, pAlreadyAsn1Encoded);
}


void PSOBuilder::setSignature(const QByteArray& pData, bool pAlreadyAsn1Encoded)
{
	static const char* const TAG_SIGNATURE = "\x5F\x37";
	mSignature = buildAsn1Structure(TAG_SIGNATURE, pData, pAlreadyAsn1Encoded);
}


CommandApdu PSOBuilder::build()
{
	static const int INS = 0x2a;

	QByteArray data;
	data.append(mCertificateBody);
	data.append(mSignature);

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
	mCaEphemeralPublicKey = buildAsn1Structure(TAG_EPHEMERAL_PUBLIC_KEY, pData);
}


void GABuilder::setPaceMappingData(const QByteArray& pData)
{
	static const char TAG_PACE_MAPPING_DATA = char(0x81);
	mPaceMappingData = buildAsn1Structure(TAG_PACE_MAPPING_DATA, pData);
}


void GABuilder::setPaceEphemeralPublicKey(const QByteArray& pData)
{
	static const char TAG_PACE_EPHEMERAL_PUBLIC_KEY = char(0x83);
	mPaceEphemeralPublicKey = buildAsn1Structure(TAG_PACE_EPHEMERAL_PUBLIC_KEY, pData);
}


void GABuilder::setPaceAuthenticationToken(const QByteArray& pData)
{
	static const char TAG_PACE_AUTHENTICATION_TOKEN = char(0x85);
	mPaceAuthenticationToken = buildAsn1Structure(TAG_PACE_AUTHENTICATION_TOKEN, pData);
}


CommandApdu GABuilder::build()
{
	static const char INS = char(0x86);
	static const char TAG_DYNAMIC_AUTHENTICATION_DATA = 0x7C;

	QByteArray data;
	if (!mCaEphemeralPublicKey.isNull())
	{
		data.append(mCaEphemeralPublicKey);
	}
	else if (!mPaceMappingData.isNull())
	{
		data.append(mPaceMappingData);
	}
	else if (!mPaceEphemeralPublicKey.isNull())
	{
		data.append(mPaceEphemeralPublicKey);
	}
	else if (!mPaceAuthenticationToken.isNull())
	{
		data.append(mPaceAuthenticationToken);
	}
	data = buildAsn1Structure(TAG_DYNAMIC_AUTHENTICATION_DATA, data);

	return CommandApdu(mClassByte, INS, 0, 0, data, Apdu::SHORT_MAX_LE);
}


ReadBinaryBuilder::ReadBinaryBuilder(uint pOffset, uint pLe)
	: CommandApduBuilder()
	, mOffset(pOffset)
	, mLe(pLe)
{
}


CommandApdu ReadBinaryBuilder::build()
{
	static const char INS = char(0xB0);
	return CommandApdu(CommandApdu::CLA, INS, (mOffset & 0xff00) >> 8, mOffset & 0xff, QByteArray(), mLe);
}


ResetRetryCounterBuilder::ResetRetryCounterBuilder(const QByteArray& pPin)
	: CommandApduBuilder()
	, mPin(pPin)
{
}


CommandApdu ResetRetryCounterBuilder::build()
{
	static const int INS = 0x2c;
	// P1: 2 (change), 3 (unblock)
	int p1 = mPin.isNull() ? 3 : 2;
	// P2: 3 (PIN) (2 (CAN) -- not used)
	// data: new PIN, when changing
	return CommandApdu(CommandApdu::CLA, INS, p1, 3, mPin);
}


QByteArray PinModifyBuilder::createChangeEidPinCommandData(unsigned int pTimeoutSeconds) const
{
	// According to ISO-7816-4, 7.5.10 RESET RETRY COUNTER command
	QByteArray abData = QByteArray().append("00").append("2C").append("02").append("03");

	return createCommandData(pTimeoutSeconds, 0x00, 0x01, 0x02, QByteArray::fromHex(abData));
}


QByteArray PinModifyBuilder::createCommandData(unsigned int pTimeoutSeconds, char pMsgIndex1, char pMsgIndex2, char pMsgIndex3, const QByteArray& pAbData) const
{
	if (pTimeoutSeconds > 255)
	{
		qCWarning(card) << "Timeout must fit in one byte, set to 255";
		pTimeoutSeconds = 255;
	}

	// as defined in PC/SC, Part 10 "IFDs with Secure PIN Entry Capabilities"
	QByteArray command;
	//	bTimeOut (timeout in seconds)
	command.append(pTimeoutSeconds)
	//	bTimeOut2 (timeout in seconds after first key pressed)
	.append(pTimeoutSeconds)
	//	bmFormatString (PIN format): system unit is bytes (0x80), ASCII format (0x02)
	.append(char(0x82))
	//	bmPINBlockString (PIN block size and length info): PIN not in APDU command
	.append(static_cast<char>(0))
	//	bmPINLengthFormat (format of PIN length field in APDU command): PIN not in APDU command
	.append(static_cast<char>(0))
	//	bInsertionOffsetOld (insertion position offset for old PIN)
	.append(static_cast<char>(0))
	//	bInsertionOffsetNew BYTE (insertion position offset for new PIN)
	.append(static_cast<char>(0))
	//	wPINMaxExtraDigit USHORT (0xXXYY, min (XX) and max (length) of new PIN)
	.append(0x06).append(0x06)
	//	bConfirmPIN (PIN confirmation options): confirm new PIN (0x01)
	.append(0x01)
	//	bEntryValidationCondition (new PIN validation options): validation key pressed (0x02)
	.append(0x02)
	//	bNumberMessage (number of display messages to be sent)
	.append(0x02)
	//	wLangId (language ID for display messages): German (0x0407)
	.append(0x07).append(0x04)
	//	bMsgIndex1 (index (into reader table) of first message to display)
	.append(pMsgIndex1)
	//	bMsgIndex2 (index (into reader table) of second message to display)
	.append(pMsgIndex2)
	//	bMsgIndex3 (index (into reader table) of third message to display)
	.append(pMsgIndex3)
	//	bTeoPrologue (T1 only: I-block prologue field to use): fill with 0
	.append(static_cast<char>(0)).append(static_cast<char>(0)).append(static_cast<char>(0));

	if (pAbData.size() > 255)
	{
		qCCritical(card) << "abData sizes greater than 255 currently not supported.";
	}
	//	ulDataLength (length of the APDU to be sent to ICC)
	command.append(pAbData.size()).append(static_cast<char>(0x00)).append(static_cast<char>(0x00)).append(static_cast<char>(0x00))
	.append(pAbData);

	return command;
}


CommandApdu PinModifyBuilder::createCommandDataCcid(unsigned int pTimeoutSeconds) const
{
	if (pTimeoutSeconds > 255)
	{
		qCWarning(card) << "Timeout must fit in one byte, set to 255";
		pTimeoutSeconds = 255;
	}

	// According to TR-03119 the command data has to be the full PC_to_RDR_Secure structure
	// According to Reiner SCT the firmware is implemented in such a way, that the command
	// data is expected as abPINOperationDataStucture

	// according to DWG_Smart-Card_CCID_Rev110.pdf
	QByteArray abPINDataStructure;

	//	bTimeOut (timeout in seconds)
	abPINDataStructure.append(pTimeoutSeconds);
	//	bmFormatString (PIN format): system unit is bytes (0x80), ASCII format (0x02)
	abPINDataStructure.append(char(0x82));
	//	bmPINBlockString (PIN block size and length info): PIN not in APDU command
	abPINDataStructure.append(static_cast<char>(0));
	//	bmPINLengthFormat (format of PIN length field in APDU command): PIN not in APDU command
	abPINDataStructure.append(static_cast<char>(0));
	//	bInsertionOffsetOld (insertion position offset for old PIN)
	abPINDataStructure.append(static_cast<char>(0));
	//	bInsertionOffsetNew BYTE (insertion position offset for new PIN)
	abPINDataStructure.append(static_cast<char>(0));
	//	wPINMaxExtraDigit USHORT (0xXXYY, min (XX) and max (length) of new PIN)
	abPINDataStructure.append(0x06).append(0x06);
	//	bConfirmPIN (PIN confirmation options): confirm new PIN (0x01)
	abPINDataStructure.append(0x01);
	//	bEntryValidationCondition (new PIN validation options): validation key pressed (0x02)
	abPINDataStructure.append(0x02);
	//	bNumberMessage (number of display messages to be sent)
	abPINDataStructure.append(0x02);
	//	wLangId (language ID for display messages): German (0x0407)
	abPINDataStructure.append(0x07).append(0x04);
	//	bMsgIndex1 (index (into reader table) of first message to display)
	abPINDataStructure.append(0x01);
	//	bMsgIndex2 (index (into reader table) of second message to display)
	abPINDataStructure.append(0x02);
	//	bMsgIndex3 (index (into reader table) of third message to display)
	abPINDataStructure.append(static_cast<char>(0));
	//	bTeoPrologue (T1 only: I-block prologue field to use): fill with 0
	abPINDataStructure.append(static_cast<char>(0)).append(static_cast<char>(0)).append(static_cast<char>(0));
	//	abData (APDU to be sent to ICC)
	abPINDataStructure.append(static_cast<char>(0x00)) // CLA: command
	.append(static_cast<char>(0x2c))         // INS: Reset Retry Counter
	.append(static_cast<char>(0x02))         // P1: new PIN/CAN
	.append(static_cast<char>(0x03));         // P2: PIN
	QByteArray abPINOperationDataStucture;
	abPINOperationDataStucture.append(0x01); //bPINOperation
	abPINOperationDataStucture.append(abPINDataStructure); //abPINDataStructure

	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x10, abPINOperationDataStucture);
}


void PinModifyOutput::parse(const QByteArray& pData)
{
	if (pData.size() != 2)
	{
		mReturnCode = ReturnCode::UNKNOWN;
	}

	const int errorCode = static_cast<quint8>(pData.at(0)) << 8 | static_cast<quint8>(pData.at(1));
	switch (errorCode)
	{
		case 0x6400:
			// operation timed out
			mReturnCode = ReturnCode::TIME_OUT;
			break;

		case 0x6401:
			// operation canceled by "Cancel" button
			mReturnCode = ReturnCode::CANCELLATION_BY_USER;
			break;

		case 0x6402:
			// the two new PIN entries don't match
			mReturnCode = ReturnCode::NEW_PINS_DONT_MATCH;
			break;

		case 0x6403:
			// entered PIN too short/long
			mReturnCode = ReturnCode::NEW_PIN_TOO_SHORT_OR_LONG;
			break;

		case 0x6b80:
			// invalid parameter in passed structure
			mReturnCode = ReturnCode::COMMAND_FAILED;
			break;

		case 0x6982:
			// terminal is not authorized to unblock or change the PIN
			mReturnCode = ReturnCode::UNKNOWN;
			break;

		case 0x9000:
			// success
			mReturnCode = ReturnCode::OK;
			break;

		default:
			qCDebug(card) << "unknown error:" << pData.toHex();
			mReturnCode = ReturnCode::UNKNOWN;
			break;
	}
}


void PinModifyOutput::parseFromCcid(const QByteArray& pData)
{
	parse(pData);
}


ReturnCode PinModifyOutput::getReturnCode() const
{
	return mReturnCode;
}
