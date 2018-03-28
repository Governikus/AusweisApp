/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "asn1/ASN1Util.h"
#include "EstablishPACEChannel.h"
#include "PersoSimWorkaround.h"

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


}


namespace governikus
{


/*
 * There is no NUMERICSTRING implementation available in the macro system of OpenSSL,
 * so we define it.
 */
ASN1_ITEM_TEMPLATE(NUMERICSTRING) =
			ASN1_EX_TEMPLATE_TYPE(ASN1_TFLG_IMPTAG, 0x12, NUMERICSTRING, ASN1_OCTET_STRING)
ASN1_ITEM_TEMPLATE_END(NUMERICSTRING)


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


EstablishPACEChannelBuilder::EstablishPACEChannelBuilder()
	: mPasswordId(PACE_PASSWORD_ID::PACE_MRZ)
	, mChat(nullptr)
	, mCertificateDescription()
{
}


void EstablishPACEChannelBuilder::setCertificateDescription(const QByteArray& pCertificateDescription)
{
	mCertificateDescription = pCertificateDescription;
}


void EstablishPACEChannelBuilder::setChat(const QByteArray& pChat)
{
	mChat = pChat;
}


void EstablishPACEChannelBuilder::setPasswordId(PACE_PASSWORD_ID pPasswordId)
{
	mPasswordId = pPasswordId;
}


QByteArray EstablishPACEChannelBuilder::createCommandData()
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


CommandApdu EstablishPACEChannelBuilder::createCommandDataCcid()
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


EstablishPACEChannelOutput::EstablishPACEChannelOutput()
	: mPaceReturnCode(CardReturnCode::UNKNOWN)
	, mEfCardAccess()
	, mCarCurr()
	, mCarPrev()
	, mIdIcc()
	, mStatusMseSetAt()
{
}


CardReturnCode EstablishPACEChannelOutput::getPaceReturnCode() const
{
	return mPaceReturnCode;
}


QByteArray EstablishPACEChannelOutput::getCARcurr() const
{
	return mCarCurr;
}


QByteArray EstablishPACEChannelOutput::getCARprev() const
{
	return mCarPrev;
}


QByteArray EstablishPACEChannelOutput::getEfCardAccess() const
{
	return mEfCardAccess;
}


QByteArray EstablishPACEChannelOutput::getIDicc() const
{
	return mIdIcc;
}


void EstablishPACEChannelOutput::setCarCurr(const QByteArray& pCarCurr)
{
	Q_ASSERT(mCarCurr.isNull());
	mCarCurr = pCarCurr;
}


void EstablishPACEChannelOutput::setCarPrev(const QByteArray& pCarPrev)
{
	Q_ASSERT(mCarPrev.isNull());
	mCarPrev = pCarPrev;
}


QByteArray EstablishPACEChannelOutput::getMseStatusSetAt() const
{
	return mStatusMseSetAt;
}


void EstablishPACEChannelOutput::setStatusMseSetAt(const QByteArray& pStatusMseSetAt)
{
	mStatusMseSetAt = pStatusMseSetAt;
}


void EstablishPACEChannelOutput::setEfCardAccess(const QByteArray& pEfCardAccess)
{
	Q_ASSERT(mEfCardAccess.isNull());
	mEfCardAccess = pEfCardAccess;
}


void EstablishPACEChannelOutput::setIdIcc(const QByteArray& pIDicc)
{
	Q_ASSERT(mIdIcc.isNull());
	mIdIcc = pIDicc;
}


void EstablishPACEChannelOutput::setPaceReturnCode(CardReturnCode pPaceReturnCode)
{
	mPaceReturnCode = pPaceReturnCode;
}


void EstablishPACEChannelOutput::parse(const QByteArray& pControlOutput, PACE_PASSWORD_ID pPasswordId)
{
	if (pControlOutput.size() < 6)
	{
		qCWarning(card) << "Output of EstablishPACEChannel has wrong size";
		return;
	}
	quint32 paceReturnCode = qFromLittleEndian<quint32>(pControlOutput.data());
	mPaceReturnCode = parseReturnCode(paceReturnCode, pPasswordId);
	if (mPaceReturnCode == CardReturnCode::UNKNOWN)
	{
		mPaceReturnCode = PersoSimWorkaround::parsingEstablishPACEChannelOutput(pControlOutput, pPasswordId);
	}

	quint16 dataLength = qFromLittleEndian<quint16>(pControlOutput.data() + 4);
	if (pControlOutput.size() < 6 + dataLength)
	{
		qCWarning(card) << "Output of EstablishPACEChannel has wrong size";
		return;
	}
	if (dataLength == 0)
	{
		qCDebug(card) << "No more data available";
		return;
	}

	// Response data according to PC/SC Part 10 amendment 1.1
	quint16 status = qFromBigEndian<quint16>(pControlOutput.data() + 6);
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


QByteArray EstablishPACEChannelOutput::toCcid() const
{
	auto establishPaceChannelOutput = newObject<ESTABLISHPACECHANNELOUTPUT>();

	QByteArray paceReturnCodeBytes;
	QDataStream(&paceReturnCodeBytes, QIODevice::WriteOnly) << Enum<EstablishPACEChannelErrorCode>::getValue(generateReturnCode(mPaceReturnCode));
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

	const uchar* unsignedCharPointer = reinterpret_cast<const uchar*>(mEfCardAccess.constData());
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


void EstablishPACEChannelOutput::parseFromCcid(const QByteArray& pOutput, PACE_PASSWORD_ID pPasswordId)
{
	mPaceReturnCode = CardReturnCode::UNKNOWN;
	mEfCardAccess.clear();
	mCarCurr.clear();
	mCarPrev.clear();
	mIdIcc.clear();
	mStatusMseSetAt.clear();

	if (pOutput.size() < 2)
	{
		qCCritical(card) << "EstablishPACEChannelOutput too short";
		return;
	}
	qCDebug(card) << "Reader returned " << pOutput.mid(pOutput.size() - 2).toHex();

	QByteArray outputData = pOutput.mid(0, pOutput.size() - 2);
	auto channelOutput = decodeObject<ESTABLISHPACECHANNELOUTPUT>(outputData);
	if (channelOutput == nullptr)
	{
		auto outputDataHex = QString::fromLatin1(outputData.toHex());
		qCCritical(card) << "Parsing EstablishPACEChannelOutput failed" << outputDataHex;

		// Try to parse the value of EstablishPACEChannelOutput.errorCode
		// the regular expression is determined by the ASN.1 structure of EstablishPACEChannelOutput

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
	qDebug() << "mPaceReturnCode:" << mPaceReturnCode << paceReturnCodeBytes.toHex();

	if (channelOutput->mStatusMSESetAt)
	{
		mStatusMseSetAt = Asn1OctetStringUtil::getValue(channelOutput->mStatusMSESetAt);
		qDebug() << "mStatusMseSetAt:" << mStatusMseSetAt.toHex();
	}

	if (channelOutput->mEfCardAccess)
	{
		mEfCardAccess = encodeObject(channelOutput->mEfCardAccess);
		qDebug() << "mEfCardAccess:" << mEfCardAccess.toHex();
	}

	if (channelOutput->mIdPICC != nullptr)
	{
		mIdIcc = Asn1OctetStringUtil::getValue(channelOutput->mIdPICC);
		qDebug() << "mIdIcc:" << mIdIcc.toHex();
	}

	if (channelOutput->mCurCAR != nullptr)
	{
		mCarCurr = Asn1OctetStringUtil::getValue(channelOutput->mCurCAR);
		qDebug() << "mCarCurr:" << mCarCurr;
	}

	if (channelOutput->mPrevCAR != nullptr)
	{
		mCarPrev = Asn1OctetStringUtil::getValue(channelOutput->mPrevCAR);
		qDebug() << "mCarPrev:" << mCarPrev;
	}
}


CardReturnCode EstablishPACEChannelOutput::parseReturnCode(quint32 pPaceReturnCode, PACE_PASSWORD_ID pPasswordId)
{
	// error codes from the reader
	switch (EstablishPACEChannelErrorCode(pPaceReturnCode))
	{
		case EstablishPACEChannelErrorCode::NoError:
			// no error
			return CardReturnCode::OK;

		case EstablishPACEChannelErrorCode::InconsistentLengthsInInput:
		case EstablishPACEChannelErrorCode::UnexpectedDataInInput:
		case EstablishPACEChannelErrorCode::UnexpectedCombinationOfDataInInput:
		case EstablishPACEChannelErrorCode::SyntaxErrorInTLVResponse:
		case EstablishPACEChannelErrorCode::UnexpectedOrMissingObjectInTLVResponse:
		case EstablishPACEChannelErrorCode::UnknownPasswordID:
		case EstablishPACEChannelErrorCode::WrongAuthenticationToken:
			return CardReturnCode::COMMAND_FAILED;

		// 0xf00663c2 -- invalid PIN?
		case EstablishPACEChannelErrorCode::CommunicationAbort:
		case EstablishPACEChannelErrorCode::NoCard:
			return CardReturnCode::COMMAND_FAILED;

		case EstablishPACEChannelErrorCode::Abort:
			return CardReturnCode::CANCELLATION_BY_USER;

		case EstablishPACEChannelErrorCode::Timeout:
			return CardReturnCode::INPUT_TIME_OUT;

		default:
			break;
	}

	// Error codes wrapping error codes from the card. The format is 0xXXXXYYZZ, where XXXX identifies
	// the command/step, and YY and ZZ encode the SW1 and SW2 from the response APDU from the card.
	switch (pPaceReturnCode & 0xffff0000)
	{
		case 0xf0000000: // Select EF.CardAccess
		case 0xf0010000: // Read Binary EF.CardAccess
		case 0xf0020000: // MSE: Set AT
			break;

		case 0xf0030000: // General Authenticate Step 1
		case 0xf0040000: // General Authenticate Step 2
		case 0xf0050000: // General Authenticate Step 3
		case 0xf0060000: // General Authenticate Step 4
			if ((pPaceReturnCode & 0xff00) == 0x6300)
			{
				// SW1 == 0x63 is a warning, which includes incorrectly entered CAN/PIN. For the PIN
				// we get SW2 == 0xcX, with X being the number of remaining retries.
				switch (pPasswordId)
				{
					case PACE_PASSWORD_ID::PACE_MRZ:
					// No separate error code (yet).
					case PACE_PASSWORD_ID::PACE_CAN:
						return CardReturnCode::INVALID_CAN;

					case PACE_PASSWORD_ID::PACE_PIN:
						return CardReturnCode::INVALID_PIN;

					case PACE_PASSWORD_ID::PACE_PUK:
						return CardReturnCode::INVALID_PUK;
				}
			}
			break;
	}

	return CardReturnCode::UNKNOWN;
}


EstablishPACEChannelErrorCode EstablishPACEChannelOutput::generateReturnCode(CardReturnCode pReturnCode)
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
			return EstablishPACEChannelErrorCode::UnexpectedDataInInput;

		case CardReturnCode::INVALID_CAN:
		case CardReturnCode::INVALID_PIN:
		case CardReturnCode::INVALID_PUK:
			return EstablishPACEChannelErrorCode::GeneralAuthenticateStep1_4_Warning;

		case CardReturnCode::OK:
			return EstablishPACEChannelErrorCode::NoError;

		case CardReturnCode::CARD_NOT_FOUND:
			return EstablishPACEChannelErrorCode::NoCard;

		case CardReturnCode::INPUT_TIME_OUT:
			return EstablishPACEChannelErrorCode::Timeout;


		case CardReturnCode::COMMAND_FAILED:
			return EstablishPACEChannelErrorCode::CommunicationAbort;

		case CardReturnCode::CANCELLATION_BY_USER:
			return EstablishPACEChannelErrorCode::Abort;
	}

	Q_UNREACHABLE();
	return EstablishPACEChannelErrorCode::UnexpectedDataInInput;
}
