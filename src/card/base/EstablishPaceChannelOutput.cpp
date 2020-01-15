/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#include "EstablishPaceChannelOutput.h"

#include "ResponseApdu.h"

#include <QDataStream>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QtEndian>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;


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


EstablishPaceChannelOutput::EstablishPaceChannelOutput(CardReturnCode pPaceReturnCode)
	: mPaceReturnCode(pPaceReturnCode)
	, mEfCardAccess()
	, mCarCurr()
	, mCarPrev()
	, mIdIcc()
	, mStatusMseSetAt()
{
}


CardReturnCode EstablishPaceChannelOutput::getPaceReturnCode() const
{
	return mPaceReturnCode;
}


QByteArray EstablishPaceChannelOutput::getCARcurr() const
{
	return mCarCurr;
}


QByteArray EstablishPaceChannelOutput::getCARprev() const
{
	return mCarPrev;
}


QByteArray EstablishPaceChannelOutput::getEfCardAccess() const
{
	return mEfCardAccess;
}


QByteArray EstablishPaceChannelOutput::getIDicc() const
{
	return mIdIcc;
}


void EstablishPaceChannelOutput::setCarCurr(const QByteArray& pCarCurr)
{
	Q_ASSERT(mCarCurr.isNull());
	mCarCurr = pCarCurr;
}


void EstablishPaceChannelOutput::setCarPrev(const QByteArray& pCarPrev)
{
	Q_ASSERT(mCarPrev.isNull());
	mCarPrev = pCarPrev;
}


QByteArray EstablishPaceChannelOutput::getMseStatusSetAt() const
{
	return mStatusMseSetAt;
}


void EstablishPaceChannelOutput::setStatusMseSetAt(const QByteArray& pStatusMseSetAt)
{
	mStatusMseSetAt = pStatusMseSetAt;
}


void EstablishPaceChannelOutput::setEfCardAccess(const QByteArray& pEfCardAccess)
{
	Q_ASSERT(mEfCardAccess.isNull());
	mEfCardAccess = pEfCardAccess;
}


void EstablishPaceChannelOutput::setIdIcc(const QByteArray& pIDicc)
{
	Q_ASSERT(mIdIcc.isNull());
	mIdIcc = pIDicc;
}


void EstablishPaceChannelOutput::setPaceReturnCode(CardReturnCode pPaceReturnCode)
{
	mPaceReturnCode = pPaceReturnCode;
}


void EstablishPaceChannelOutput::parse(const QByteArray& pControlOutput, PacePasswordId pPasswordId)
{
	if (pControlOutput.size() < 6)
	{
		qCWarning(card) << "Output of EstablishPaceChannel has wrong size";
		return;
	}
	auto paceReturnCode = qFromLittleEndian<quint32>(pControlOutput.data());
	mPaceReturnCode = parseReturnCode(paceReturnCode, pPasswordId);

	auto dataLength = qFromLittleEndian<quint16>(pControlOutput.data() + 4);
	if (pControlOutput.size() < 6 + dataLength)
	{
		qCWarning(card) << "Output of EstablishPaceChannel has wrong size";
		return;
	}
	if (dataLength == 0)
	{
		qCDebug(card) << "No more data available";
		return;
	}

	// Response data according to PC/SC Part 10 amendment 1.1
	auto status = qFromBigEndian<quint16>(pControlOutput.data() + 6);
	if (status != StatusCode::SUCCESS)
	{
		qCWarning(card) << "PACE failed. Status code:" << status;
	}

	int it = 8;
	mEfCardAccess = readByteArray<quint16>(pControlOutput, it);

	if (it >= pControlOutput.size())
	{
		// in case of managing eSign PIN no CAR or IdICC is contained
		qCDebug(card) << "No CAR or IdICC contained";
		return;
	}

	mCarCurr = readByteArray<quint8>(pControlOutput, it);
	qCDebug(card) << "mCarCurr:" << mCarCurr;
	mCarPrev = readByteArray<quint8>(pControlOutput, it);
	qCDebug(card) << "mCarPrev:" << mCarPrev;
	mIdIcc = readByteArray<quint16>(pControlOutput, it);
	qCDebug(card) << "mIdIcc:" << mIdIcc.toHex();
}


QByteArray EstablishPaceChannelOutput::toCcid() const
{
	auto establishPaceChannelOutput = newObject<ESTABLISHPACECHANNELOUTPUT>();

	QByteArray paceReturnCodeBytes;
	QDataStream(&paceReturnCodeBytes, QIODevice::WriteOnly) << Enum<EstablishPaceChannelErrorCode>::getValue(generateReturnCode(mPaceReturnCode));
	establishPaceChannelOutput->mErrorCode = ASN1_OCTET_STRING_new();
	Asn1OctetStringUtil::setValue(paceReturnCodeBytes, establishPaceChannelOutput->mErrorCode);

	establishPaceChannelOutput->mStatusMSESetAt = ASN1_OCTET_STRING_new();
	if (mStatusMseSetAt.isEmpty())
	{
		qCWarning(card) << "mStatusMseSetAt is empty! Using 0000 as dummy...";
		Asn1OctetStringUtil::setValue(QByteArray::fromHex(QByteArrayLiteral("0000")), establishPaceChannelOutput->mStatusMSESetAt);
	}
	else
	{
		Asn1OctetStringUtil::setValue(mStatusMseSetAt, establishPaceChannelOutput->mStatusMSESetAt);
	}

	const auto* unsignedCharPointer = reinterpret_cast<const uchar*>(mEfCardAccess.constData());
	decodeAsn1Object(&establishPaceChannelOutput->mEfCardAccess, &unsignedCharPointer, mEfCardAccess.size());

	establishPaceChannelOutput->mIdPICC = ASN1_OCTET_STRING_new();
	Asn1OctetStringUtil::setValue(mIdIcc, establishPaceChannelOutput->mIdPICC);
	establishPaceChannelOutput->mCurCAR = ASN1_OCTET_STRING_new();
	Asn1OctetStringUtil::setValue(mCarCurr, establishPaceChannelOutput->mCurCAR);
	establishPaceChannelOutput->mPrevCAR = ASN1_OCTET_STRING_new();
	Asn1OctetStringUtil::setValue(mCarPrev, establishPaceChannelOutput->mPrevCAR);

	QByteArray ccidOutput = encodeObject(establishPaceChannelOutput.data());

	QByteArray ccidErrorCode;
	QDataStream(&ccidErrorCode, QIODevice::WriteOnly) << Enum<StatusCode>::getValue(StatusCode::SUCCESS);
	ccidOutput += ccidErrorCode;

	return ccidOutput;
}


void EstablishPaceChannelOutput::parseFromCcid(const QByteArray& pOutput, PacePasswordId pPasswordId)
{
	mPaceReturnCode = CardReturnCode::UNKNOWN;
	mEfCardAccess.clear();
	mCarCurr.clear();
	mCarPrev.clear();
	mIdIcc.clear();
	mStatusMseSetAt.clear();

	if (pOutput.size() < 2)
	{
		qCCritical(card) << "EstablishPaceChannelOutput too short";
		return;
	}
	qCDebug(card) << "Reader returned:" << pOutput.mid(pOutput.size() - 2).toHex();

	QByteArray outputData = pOutput.mid(0, pOutput.size() - 2);
	auto channelOutput = decodeObject<ESTABLISHPACECHANNELOUTPUT>(outputData);
	if (channelOutput == nullptr)
	{
		auto outputDataHex = QString::fromLatin1(outputData.toHex());
		qCCritical(card) << "Parsing EstablishPaceChannelOutput failed" << outputDataHex;

		// Try to parse the value of EstablishPaceChannelOutput.errorCode
		// the regular expression is determined by the ASN.1 structure of EstablishPaceChannelOutput

		QRegularExpression regExp(QStringLiteral("(.*)a1060404(?<a1>([[:xdigit:]]){8})a2040402"));
		auto match = regExp.match(outputDataHex);
		if (match.hasMatch())
		{
			qCWarning(card) << "Determine at least PACE return code by regular expression";
			QByteArray paceReturnCodeBytes = QByteArray::fromHex(match.captured(QStringLiteral("a1")).toUtf8());
			quint32 paceReturnCode;
			QDataStream(paceReturnCodeBytes) >> paceReturnCode;
			mPaceReturnCode = parseReturnCode(paceReturnCode, pPasswordId);
			qCDebug(card) << "mPaceReturnCode:" << mPaceReturnCode << paceReturnCodeBytes.toHex();
		}
		return;
	}

	QByteArray paceReturnCodeBytes = Asn1OctetStringUtil::getValue(channelOutput->mErrorCode);
	quint32 paceReturnCode;
	QDataStream(paceReturnCodeBytes) >> paceReturnCode;
	mPaceReturnCode = parseReturnCode(paceReturnCode, pPasswordId);
	qCDebug(card) << "mPaceReturnCode:" << mPaceReturnCode << paceReturnCodeBytes.toHex();

	if (channelOutput->mStatusMSESetAt)
	{
		mStatusMseSetAt = Asn1OctetStringUtil::getValue(channelOutput->mStatusMSESetAt);
		qCDebug(card) << "mStatusMseSetAt:" << mStatusMseSetAt.toHex();
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
}


CardReturnCode EstablishPaceChannelOutput::parseReturnCode(quint32 pPaceReturnCode, PacePasswordId pPasswordId)
{
	// error codes from the reader
	switch (EstablishPaceChannelErrorCode(pPaceReturnCode))
	{
		case EstablishPaceChannelErrorCode::NoError:
			// no error
			return CardReturnCode::OK;

		case EstablishPaceChannelErrorCode::InconsistentLengthsInInput:
		case EstablishPaceChannelErrorCode::UnexpectedDataInInput:
		case EstablishPaceChannelErrorCode::UnexpectedCombinationOfDataInInput:
		case EstablishPaceChannelErrorCode::SyntaxErrorInTLVResponse:
		case EstablishPaceChannelErrorCode::UnexpectedOrMissingObjectInTLVResponse:
		case EstablishPaceChannelErrorCode::UnknownPasswordID:
		case EstablishPaceChannelErrorCode::WrongAuthenticationToken:
			return CardReturnCode::COMMAND_FAILED;

		// 0xf00663c2 -- invalid PIN?
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
	switch (pPaceReturnCode & 0xffff0000U)
	{
		case 0xf0000000U: // Select EF.CardAccess
		case 0xf0010000U: // Read Binary EF.CardAccess
		case 0xf0020000U: // MSE: Set AT
			break;

		case 0xf0030000U: // General Authenticate Step 1
		case 0xf0040000U: // General Authenticate Step 2
		case 0xf0050000U: // General Authenticate Step 3
		case 0xf0060000U: // General Authenticate Step 4
			if ((pPaceReturnCode & 0xff00U) == 0x6300U)
			{
				// SW1 == 0x63 is a warning, which includes incorrectly entered CAN/PIN. For the PIN
				// we get SW2 == 0xcX, with X being the number of remaining retries.
				switch (pPasswordId)
				{
					case PacePasswordId::PACE_MRZ:
					// No separate error code (yet).
					case PacePasswordId::PACE_CAN:
						return CardReturnCode::INVALID_CAN;

					case PacePasswordId::PACE_PIN:
						return CardReturnCode::INVALID_PIN;

					case PacePasswordId::PACE_PUK:
						return CardReturnCode::INVALID_PUK;

					case PacePasswordId::UNKNOWN:
						return CardReturnCode::UNKNOWN;
				}
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
			return EstablishPaceChannelErrorCode::UnexpectedDataInInput;

		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PIN_2:
		case CardReturnCode::INVALID_PIN_3:
		case CardReturnCode::INVALID_PUK:
			return EstablishPaceChannelErrorCode::GeneralAuthenticateStep1_4_Warning;

		case CardReturnCode::OK:
		case CardReturnCode::OK_PUK:
			return EstablishPaceChannelErrorCode::NoError;

		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::RETRY_ALLOWED:
			return EstablishPaceChannelErrorCode::NoCard;

		case CardReturnCode::INPUT_TIME_OUT:
			return EstablishPaceChannelErrorCode::Timeout;

		case CardReturnCode::COMMAND_FAILED:
			return EstablishPaceChannelErrorCode::CommunicationAbort;

		case CardReturnCode::CANCELLATION_BY_USER:
			return EstablishPaceChannelErrorCode::Abort;
	}

	Q_UNREACHABLE();
	return EstablishPaceChannelErrorCode::UnexpectedDataInInput;
}
