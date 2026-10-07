/**
 * Copyright (c) 2015-2026 Governikus Service GmbH, Germany
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


CardReturnCode EstablishPaceChannelOutput::toReturnCode(quint32 pErrorCode)
{
	// error codes from the reader
	switch (EstablishPaceChannelErrorCode(pErrorCode))
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

		case EstablishPaceChannelErrorCode::CommunicationAbort:
			return CardReturnCode::COMMAND_FAILED;

		case EstablishPaceChannelErrorCode::NoCard:
			return CardReturnCode::CARD_NOT_FOUND;

		case EstablishPaceChannelErrorCode::Abort:
			return CardReturnCode::CANCELLATION_BY_USER;

		case EstablishPaceChannelErrorCode::Timeout:
			return CardReturnCode::INPUT_TIME_OUT;

		default:
		{
			// Error codes wrapping error codes from the card. The format is 0xXXXXYYZZ, where XXXX identifies
			// the command/step, and YY and ZZ encode the SW1 and SW2 from the response APDU from the card.
			if (EstablishPaceChannelErrorCode(pErrorCode & 0xFFFFFF00U) == EstablishPaceChannelErrorCode::GeneralAuthenticateStep4)
			{
				return CardReturnCode::OK;
			}

			return CardReturnCode::UNKNOWN;
		}
	}
}


void EstablishPaceChannelOutput::initMseStatusSetAt()
{
	mStatusMseSetAt = QByteArray::fromHex("0000");
}


void EstablishPaceChannelOutput::initEfCardAccess()
{
	mEfCardAccess = QByteArray::fromHex("3100");
}


EstablishPaceChannelOutput::EstablishPaceChannelOutput(PacePasswordId pPasswordId, CardReturnCode pReturnCode)
	: mPasswordId(pPasswordId)
	, mReturnCode(pReturnCode)
	, mErrorCode(0)
	, mStatusMseSetAt()
	, mEfCardAccess()
	, mIdIcc()
	, mCarCurr()
	, mCarPrev()
{
	setReturnCode(pReturnCode);
	initMseStatusSetAt();
	initEfCardAccess();
}


bool EstablishPaceChannelOutput::parse(const QByteArray& pControlOutput)
{
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

	if (pControlOutput.size() < 6)
	{
		setReturnCode(CardReturnCode::COMMAND_FAILED);
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
	if (pPaceOutput.size() < 4)
	{
		mReturnCode = CardReturnCode::COMMAND_FAILED;
		return false;
	}

	// PCSC Part 10 section 2.5.1: "Byte ordering is decided by machine architecture."
	mErrorCode = qFromLittleEndian<quint32>(pPaceOutput.data());
	mReturnCode = toReturnCode(mErrorCode);
	qCDebug(card) << "mPaceReturnCode:" << pPaceOutput.mid(0, 4).toHex() << mReturnCode;

	return true;
}


bool EstablishPaceChannelOutput::parseOutputData(const QByteArray& pOutput)
{
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

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
	initMseStatusSetAt();
	initEfCardAccess();
	mIdIcc.clear();
	mCarCurr.clear();
	mCarPrev.clear();

	if (pOutput.size() < 2)
	{
		mReturnCode = CardReturnCode::COMMAND_FAILED;
		qCCritical(card) << "EstablishPaceChannelOutput too short";
		return false;
	}
	qCDebug(card) << "Reader returned:" << pOutput.mid(pOutput.size() - 2).toHex();

	const QByteArray& outputData = pOutput.mid(0, pOutput.size() - 2);
	const auto channelOutput = decodeObject<ESTABLISHPACECHANNELOUTPUT>(outputData);
	if (channelOutput == nullptr)
	{
		mReturnCode = CardReturnCode::COMMAND_FAILED;
		const auto& outputDataHex = QString::fromLatin1(outputData.toHex());
		qCCritical(card) << "Parsing EstablishPaceChannelOutput failed" << outputDataHex;
		return false;
	}

	const QByteArray paceReturnCodeBytes = Asn1OctetStringUtil::getValue(channelOutput->mErrorCode);
	mErrorCode = qFromBigEndian<quint32>(paceReturnCodeBytes.data());
	mReturnCode = toReturnCode(mErrorCode);
	qCDebug(card) << "mPaceReturnCode:" << paceReturnCodeBytes.toHex() << mReturnCode;

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


PacePasswordId EstablishPaceChannelOutput::getPasswordId() const
{
	return mPasswordId;
}


CardReturnCode EstablishPaceChannelOutput::getReturnCode() const
{
	return mReturnCode;
}


void EstablishPaceChannelOutput::setReturnCode(CardReturnCode pReturnCode)
{
	mReturnCode = pReturnCode;

	EstablishPaceChannelErrorCode errorCode = EstablishPaceChannelErrorCode::NoError;
	switch (pReturnCode)
	{
		case CardReturnCode::UNKNOWN:
		case CardReturnCode::UNDEFINED:
		case CardReturnCode::PIN_NOT_BLOCKED:
		case CardReturnCode::UNEXPECTED_TRANSMIT_STATUS:
		case CardReturnCode::PROTOCOL_ERROR:
		case CardReturnCode::WRONG_LENGTH:
			errorCode = EstablishPaceChannelErrorCode::UnexpectedDataInInput;
			break;

		case CardReturnCode::OK:
			errorCode = EstablishPaceChannelErrorCode::NoError;
			break;

		case CardReturnCode::CARD_NOT_FOUND:
		case CardReturnCode::RESPONSE_EMPTY:
			errorCode = EstablishPaceChannelErrorCode::NoCard;
			break;

		case CardReturnCode::INPUT_TIME_OUT:
			errorCode = EstablishPaceChannelErrorCode::Timeout;
			break;

		case CardReturnCode::COMMAND_FAILED:
			errorCode = EstablishPaceChannelErrorCode::CommunicationAbort;
			break;

		case CardReturnCode::CANCELLATION_BY_USER:
			errorCode = EstablishPaceChannelErrorCode::Abort;
			break;
	}
	setErrorCode(errorCode);
}


void EstablishPaceChannelOutput::setErrorCode(EstablishPaceChannelErrorCode pErrorCode)
{
	mErrorCode = Enum<EstablishPaceChannelErrorCode>::getValue(pErrorCode);
}


PaceResult EstablishPaceChannelOutput::getPaceResult() const
{
	if (mReturnCode != CardReturnCode::OK)
	{
		return PaceResult::UNDEFINED;
	}

	const auto rightPassword = EstablishPaceChannelErrorCode(mErrorCode) == EstablishPaceChannelErrorCode::NoError;
	switch (mPasswordId)
	{
		case PacePasswordId::PACE_CAN:
			if (rightPassword)
			{
				return mCarCurr.isEmpty() ? PaceResult::OK_CAN : PaceResult::OK_CAN_AUTH;
			}
			return PaceResult::INVALID_CAN;

		case PacePasswordId::PACE_PIN:
			if (rightPassword)
			{
				return mCarCurr.isEmpty() ? PaceResult::OK_PIN : PaceResult::OK_PIN_AUTH;
			}

			switch (mErrorCode & 0x0F)
			{
				case 2:
					return PaceResult::INVALID_PIN_1;

				case 1:
					return PaceResult::INVALID_PIN_2;

				default:
					return PaceResult::INVALID_PIN_3;
			}

		case PacePasswordId::PACE_PUK:
			return rightPassword ? PaceResult::OK_PUK : PaceResult::INVALID_PUK;

		default:
			return PaceResult::UNDEFINED;
	}
}


bool EstablishPaceChannelOutput::isUndefined() const
{
	return mReturnCode == CardReturnCode::UNDEFINED;
}


bool EstablishPaceChannelOutput::isOk() const
{
	if (mReturnCode == CardReturnCode::OK)
	{
		return EstablishPaceChannelErrorCode(mErrorCode) == EstablishPaceChannelErrorCode::NoError;
	}

	return false;
}


bool EstablishPaceChannelOutput::wrongPasswordUsed() const
{
	if (mReturnCode == CardReturnCode::OK)
	{
		return (mErrorCode & 0xFFFFFF00U) == EstablishPaceChannelErrorCode::GeneralAuthenticateStep4;
	}

	return false;
}


StatusCode EstablishPaceChannelOutput::getStatusCodeMseSetAt() const
{
	return ResponseApdu(mStatusMseSetAt).getStatusCode();
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
	qToLittleEndian(mErrorCode, paceReturnCodeBytes.data());
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
	qToBigEndian(mErrorCode, paceReturnCodeBytes.data());
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
