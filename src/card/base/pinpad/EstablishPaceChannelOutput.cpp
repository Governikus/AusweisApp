/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannelOutput.h"

#include "LengthValue.h"
#include "asn1/ASN1Util.h"

#include <QDataStream>
#include <QIODevice>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QtEndian>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


namespace governikus
{

ASN1_SEQUENCE(ESTABLISHPACECHANNELOUTPUT) = {
	ASN1_EXP(ESTABLISHPACECHANNELOUTPUT, mErrorCode, ASN1_OCTET_STRING, 0x01),
	ASN1_EXP(ESTABLISHPACECHANNELOUTPUT, mStatusMSESetAt, ASN1_OCTET_STRING, 0x02),
	ASN1_EXP(ESTABLISHPACECHANNELOUTPUT, mEfCardAccess, securityinfos_st, 0x03),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELOUTPUT, mIdPICC, ASN1_OCTET_STRING, 0x04),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELOUTPUT, mCurCAR, ASN1_OCTET_STRING, 0x05),
	ASN1_EXP_OPT(ESTABLISHPACECHANNELOUTPUT, mPrevCAR, ASN1_OCTET_STRING, 0x06)
}


ASN1_SEQUENCE_END(ESTABLISHPACECHANNELOUTPUT)
IMPLEMENT_ASN1_FUNCTIONS(ESTABLISHPACECHANNELOUTPUT)
IMPLEMENT_ASN1_OBJECT(ESTABLISHPACECHANNELOUTPUT)

}  // namespace governikus


CardReturnCode EstablishPaceChannelOutput::parseReturnCode(quint32 pPaceReturnCode)
{
	// error codes from the reader
	switch (EstablishPaceChannelErrorCode(pPaceReturnCode))
	{
		case EstablishPaceChannelErrorCode::NoError:
			// no error
			return CardReturnCode::OK;

		case EstablishPaceChannelErrorCode::NoActivePinSet:
		case EstablishPaceChannelErrorCode::InconsistentLengthsInInput:
		case EstablishPaceChannelErrorCode::UnexpectedDataInInput:
		case EstablishPaceChannelErrorCode::UnexpectedCombinationOfDataInInput:
		case EstablishPaceChannelErrorCode::SyntaxErrorInTLVResponse:
		case EstablishPaceChannelErrorCode::UnexpectedOrMissingObjectInTLVResponse:
		case EstablishPaceChannelErrorCode::UnknownPasswordID:
		case EstablishPaceChannelErrorCode::WrongAuthenticationToken:
			return CardReturnCode::COMMAND_FAILED;

		// 0xF00663C2 -- invalid PIN?
		case EstablishPaceChannelErrorCode::CommunicationAbort:
			return CardReturnCode::COMMAND_FAILED;

		case EstablishPaceChannelErrorCode::NoCard:
			return CardReturnCode::CARD_NOT_FOUND;

		case EstablishPaceChannelErrorCode::Abort:
			return CardReturnCode::CANCELLATION_BY_USER;

		case EstablishPaceChannelErrorCode::Timeout:
			return CardReturnCode::INPUT_TIME_OUT;

		default:
			break;
	}

	// Error codes wrapping error codes from the card. The format is 0xXXXXYYZZ, where XXXX identifies
	// the command/step, and YY and ZZ encode the SW1 and SW2 from the response APDU from the card.
	switch (pPaceReturnCode & 0xFFFF0000U)
	{
		case 0xF0000000U: // Select EF.CardAccess
		case 0xF0010000U: // Read Binary EF.CardAccess
		case 0xF0020000U: // MSE: Set AT
			break;

		case 0xF0030000U: // General Authenticate Step 1
		case 0xF0040000U: // General Authenticate Step 2
		case 0xF0050000U: // General Authenticate Step 3
		case 0xF0060000U: // General Authenticate Step 4
			if ((pPaceReturnCode & 0xFF00U) == 0x6300U)
			{
				// SW1 == 0x63 is a warning, which includes incorrectly entered CAN/PIN. For the PIN
				// we get SW2 == 0xCX, with X being the number of remaining retries.
				return CardReturnCode::INVALID_PASSWORD;
			}
			break;
	}

	return CardReturnCode::UNKNOWN;
}


EstablishPaceChannelErrorCode EstablishPaceChannelOutput::generateReturnCode(CardReturnCode pReturnCode)
{
	switch (pReturnCode)
	{
		case CardReturnCode::UNKNOWN:
		case CardReturnCode::UNDEFINED:
		case CardReturnCode::NEW_PIN_MISMATCH:
		case CardReturnCode::NEW_PIN_INVALID_LENGTH:
		case CardReturnCode::PIN_BLOCKED:
		case CardReturnCode::PIN_NOT_BLOCKED:
		case CardReturnCode::PUK_INOPERATIVE:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::WRONG_LENGTH:
			return EstablishPaceChannelErrorCode::UnexpectedDataInInput;

		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PASSWORD:
		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_PIN_3:
		case CardReturnCode::INVALID_PUK:
			return EstablishPaceChannelErrorCode::GeneralAuthenticateStep1_4_Warning;

		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
		case CardReturnCode::OK_CAN:
			return EstablishPaceChannelErrorCode::NoError;

		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::RESPONSE_EMPTY:
			return EstablishPaceChannelErrorCode::NoCard;

		case CardReturnCode::INPUT_TIME_OUT:
			return EstablishPaceChannelErrorCode::Timeout;

		case CardReturnCode::COMMAND_FAILED:
			return EstablishPaceChannelErrorCode::CommunicationAbort;

		case CardReturnCode::CANCELLATION_BY_USER:
			return EstablishPaceChannelErrorCode::Abort;
	}

	Q_UNREACHABLE();
}


void EstablishPaceChannelOutput::initMseStatusSetAt()
{
	mStatusMseSetAt = QByteArray::fromHex("0000");
}


void EstablishPaceChannelOutput::initEfCardAccess()
{
	mEfCardAccess = QByteArray::fromHex("3100");
}


bool EstablishPaceChannelOutput::findErrorCode(const QString& pOutputData)
{
	// Try to parse the value of EstablishPaceChannelOutput.errorCode
	// the regular expression is determined by the ASN.1 structure of EstablishPaceChannelOutput

	QRegularExpression regExp(QStringLiteral("(.*)a1060404(?<a1>([[:xdigit:]]){8})a2040402"));
	auto match = regExp.match(pOutputData);
	if (!match.hasMatch())
	{
		return false;
	}

	qCWarning(card) << "Determine at least PACE return code by regular expression";
	const QByteArray paceReturnCodeBytes = QByteArray::fromHex(match.captured(QStringLiteral("a1")).toUtf8());
	mPaceReturnCode = parseReturnCode(qFromBigEndian<quint32>(paceReturnCodeBytes.data()));
	qCDebug(card) << "mPaceReturnCode:" << paceReturnCodeBytes.toHex() << mPaceReturnCode;

	return true;
}


EstablishPaceChannelOutput::EstablishPaceChannelOutput(CardReturnCode pPaceReturnCode)
	: mPaceReturnCode(pPaceReturnCode)
	, mStatusMseSetAt()
	, mEfCardAccess()
	, mIdIcc()
	, mCarCurr()
	, mCarPrev()
{
	initMseStatusSetAt();
	initEfCardAccess();
}


bool EstablishPaceChannelOutput::parse(const QByteArray& pControlOutput)
{
	mPaceReturnCode = CardReturnCode::COMMAND_FAILED;
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

	if (pControlOutput.size() < 6)
	{
		qCWarning(card) << "Output of EstablishPaceChannel has wrong size";
		return false;
	}

	const bool parseResult = parseResultCode(pControlOutput);

	const auto dataLength = qFromLittleEndian<quint16>(pControlOutput.data() + 4);
	if (pControlOutput.size() < 6 + dataLength)
	{
		qCWarning(card) << "Output of EstablishPaceChannel has wrong size";
		return false;
	}

	return parseOutputData(pControlOutput.mid(6, dataLength)) && parseResult;
}


bool EstablishPaceChannelOutput::parseResultCode(const QByteArray& pPaceOutput)
{
	mPaceReturnCode = CardReturnCode::COMMAND_FAILED;

	if (pPaceOutput.size() < 4)
	{
		return false;
	}

	// PCSC Part 10 section 2.5.1: "Byte ordering is decided by machine architecture."
	const auto paceReturnCode = qFromLittleEndian<quint32>(pPaceOutput.data());
	mPaceReturnCode = parseReturnCode(paceReturnCode);
	qCDebug(card) << "mPaceReturnCode:" << pPaceOutput.mid(0, 4).toHex() << mPaceReturnCode;

	return true;
}


bool EstablishPaceChannelOutput::parseOutputData(const QByteArray& pOutput)
{
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

	if (pOutput.isEmpty())
	{
		qCDebug(card) << "No more data available";
		return true;
	}

	if (pOutput.size() < 4)
	{
		qCCritical(card) << "OutputData too short";
		return false;
	}

	// Response data according to PC/SC Part 10 amendment 1.1
	mStatusMseSetAt = pOutput.mid(0, 2);
	qCDebug(card) << "mStatusMseSetAt:" << mStatusMseSetAt.toHex() << getStatusCodeMseSetAt();

	int it = 2;
	const auto& efCardAccess = LengthValue::readByteArray<quint16>(pOutput, it);
	if (!efCardAccess.isEmpty())
	{
		mEfCardAccess = efCardAccess;
	}
	qCDebug(card) << "mEfCardAccess:" << mEfCardAccess.toHex();

	if (it == pOutput.size())
	{
		// in case of managing eSign PIN no CAR or IdICC is contained
		qCDebug(card) << "No CAR or IdICC contained";
		return true;
	}

	auto debugGuard = qScopeGuard([] {
				qCDebug(card) << "Decapsulation of command failed. Wrong size.";
			});

	if (it > pOutput.size())
	{
		return false;
	}

	mCarCurr = LengthValue::readByteArray<quint8>(pOutput, it);
	qCDebug(card) << "mCarCurr:" << mCarCurr;
	if (it > pOutput.size())
	{
		return false;
	}

	mCarPrev = LengthValue::readByteArray<quint8>(pOutput, it);
	qCDebug(card) << "mCarPrev:" << mCarPrev;
	if (it > pOutput.size())
	{
		return false;
	}

	mIdIcc = LengthValue::readByteArray<quint16>(pOutput, it);
	qCDebug(card) << "mIdIcc:" << mIdIcc.toHex();
	if (it != pOutput.size())
	{
		return false;
	}

	debugGuard.dismiss();
	return true;
}


bool EstablishPaceChannelOutput::parseFromCcid(const QByteArray& pOutput)
{
	mPaceReturnCode = CardReturnCode::COMMAND_FAILED;
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

	if (pOutput.size() < 2)
	{
		qCCritical(card) << "EstablishPaceChannelOutput too short";
		return false;
	}
	qCDebug(card) << "Reader returned:" << pOutput.mid(pOutput.size() - 2).toHex();

	const QByteArray& outputData = pOutput.mid(0, pOutput.size() - 2);
	const auto channelOutput = decodeObject<ESTABLISHPACECHANNELOUTPUT>(outputData);
	if (channelOutput == nullptr)
	{
		const auto& outputDataHex = QString::fromLatin1(outputData.toHex());
		qCCritical(card) << "Parsing EstablishPaceChannelOutput failed" << outputDataHex;
		return findErrorCode(outputDataHex);
	}

	const QByteArray paceReturnCodeBytes = Asn1OctetStringUtil::getValue(channelOutput->mErrorCode);
	mPaceReturnCode = parseReturnCode(qFromBigEndian<quint32>(paceReturnCodeBytes.data()));
	qCDebug(card) << "mPaceReturnCode:" << paceReturnCodeBytes.toHex() << mPaceReturnCode;

	if (channelOutput->mStatusMSESetAt)
	{
		mStatusMseSetAt = Asn1OctetStringUtil::getValue(channelOutput->mStatusMSESetAt);
		qCDebug(card) << "mStatusMseSetAt:" << mStatusMseSetAt.toHex() << getStatusCodeMseSetAt();
	}

	if (channelOutput->mEfCardAccess)
	{
		mEfCardAccess = encodeObject(channelOutput->mEfCardAccess);
		qCDebug(card) << "mEfCardAccess:" << mEfCardAccess.toHex();
	}

	if (channelOutput->mIdPICC != nullptr)
	{
		mIdIcc = Asn1OctetStringUtil::getValue(channelOutput->mIdPICC);
		qCDebug(card) << "mIdIcc:" << mIdIcc.toHex();
	}

	if (channelOutput->mCurCAR != nullptr)
	{
		mCarCurr = Asn1OctetStringUtil::getValue(channelOutput->mCurCAR);
		qCDebug(card) << "mCarCurr:" << mCarCurr;
	}

	if (channelOutput->mPrevCAR != nullptr)
	{
		mCarPrev = Asn1OctetStringUtil::getValue(channelOutput->mPrevCAR);
		qCDebug(card) << "mCarPrev:" << mCarPrev;
	}

	return true;
}


CardReturnCode EstablishPaceChannelOutput::getPaceReturnCode() const
{
	return mPaceReturnCode;
}


void EstablishPaceChannelOutput::setPaceReturnCode(CardReturnCode pPaceReturnCode)
{
	mPaceReturnCode = pPaceReturnCode;
}


StatusCode EstablishPaceChannelOutput::getStatusCodeMseSetAt() const
{
	return ResponseApdu(mStatusMseSetAt).getStatusCode();
}


const QByteArray& EstablishPaceChannelOutput::getStatusMseSetAt() const
{
	return mStatusMseSetAt;
}


void EstablishPaceChannelOutput::setStatusMseSetAt(const QByteArray& pStatusMseSetAt)
{
	if (pStatusMseSetAt.isEmpty())
	{
		initMseStatusSetAt();
		return;
	}

	mStatusMseSetAt = pStatusMseSetAt;
}


const QByteArray& EstablishPaceChannelOutput::getEfCardAccess() const
{
	return mEfCardAccess;
}


void EstablishPaceChannelOutput::setEfCardAccess(const QByteArray& pEfCardAccess)
{
	if (pEfCardAccess.isEmpty())
	{
		initEfCardAccess();
		return;
	}

	mEfCardAccess = pEfCardAccess;
}


const QByteArray& EstablishPaceChannelOutput::getIdIcc() const
{
	return mIdIcc;
}


void EstablishPaceChannelOutput::setIdIcc(const QByteArray& pIdIcc)
{
	mIdIcc = pIdIcc;
}


const QByteArray& EstablishPaceChannelOutput::getCarCurr() const
{
	return mCarCurr;
}


void EstablishPaceChannelOutput::setCarCurr(const QByteArray& pCarCurr)
{
	mCarCurr = pCarCurr;
}


const QByteArray& EstablishPaceChannelOutput::getCarPrev() const
{
	return mCarPrev;
}


void EstablishPaceChannelOutput::setCarPrev(const QByteArray& pCarPrev)
{
	mCarPrev = pCarPrev;
}


QByteArray EstablishPaceChannelOutput::toResultCode() const
{
	QByteArray paceReturnCodeBytes(sizeof(quint32), 0);
	quint32 value = Enum<EstablishPaceChannelErrorCode>::getValue(generateReturnCode(mPaceReturnCode));
	qToLittleEndian(value, paceReturnCodeBytes.data());
	return paceReturnCodeBytes;
}


QByteArray EstablishPaceChannelOutput::toOutputData() const
{
	QByteArray outputData;

	outputData += mStatusMseSetAt;
	LengthValue::writeByteArray<quint16>(mEfCardAccess, outputData);

	if (!mCarCurr.isEmpty() || !mCarPrev.isEmpty() || !mIdIcc.isEmpty())
	{
		LengthValue::writeByteArray<quint8>(mCarCurr, outputData);
	}

	if (!mCarPrev.isEmpty() || !mIdIcc.isEmpty())
	{
		LengthValue::writeByteArray<quint8>(mCarPrev, outputData);
	}

	if (!mIdIcc.isEmpty())
	{
		LengthValue::writeByteArray<quint16>(mIdIcc, outputData);
	}

	return outputData;
}


QByteArray EstablishPaceChannelOutput::toCcid() const
{
	auto establishPaceChannelOutput = newObject<ESTABLISHPACECHANNELOUTPUT>();

	QByteArray paceReturnCodeBytes(sizeof(quint32), 0);
	quint32 value = Enum<EstablishPaceChannelErrorCode>::getValue(generateReturnCode(mPaceReturnCode));
	qToBigEndian(value, paceReturnCodeBytes.data());
	Asn1OctetStringUtil::setValue(paceReturnCodeBytes, establishPaceChannelOutput->mErrorCode);

	Asn1OctetStringUtil::setValue(mStatusMseSetAt, establishPaceChannelOutput->mStatusMSESetAt);

	const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(mEfCardAccess.constData());
	decodeAsn1Object(&establishPaceChannelOutput->mEfCardAccess, &unsignedCharPointer, static_cast<long>(mEfCardAccess.size()));

	if (!mIdIcc.isEmpty())
	{
		establishPaceChannelOutput->mIdPICC = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mIdIcc, establishPaceChannelOutput->mIdPICC);
	}
	if (!mCarCurr.isEmpty())
	{
		establishPaceChannelOutput->mCurCAR = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mCarCurr, establishPaceChannelOutput->mCurCAR);
	}
	if (!mCarPrev.isEmpty())
	{
		establishPaceChannelOutput->mPrevCAR = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mCarPrev, establishPaceChannelOutput->mPrevCAR);
	}

	QByteArray ccidOutput = encodeObject(establishPaceChannelOutput.data());

	QByteArray ccidErrorCode;
	QDataStream(&ccidErrorCode, QIODevice::WriteOnly) << Enum<StatusCode>::getValue(StatusCode::SUCCESS);
	ccidOutput += ccidErrorCode;

	return ccidOutput;
}
