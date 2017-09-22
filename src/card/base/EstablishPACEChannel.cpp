/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "asn1/ASN1Util.h"
#include "EstablishPACEChannel.h"
#include "PersoSimWorkaround.h"

#include <QDataStream>
#include <QLoggingCategory>
#include <QRegularExpression>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


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
	: mPinId(PACE_PIN_ID::PACE_MRZ)
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


void EstablishPACEChannelBuilder::setPinId(PACE_PIN_ID pPinId)
{
	mPinId = pPinId;
}


QByteArray EstablishPACEChannelBuilder::createCommandData()
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_ESTABLISH_PACE_CHANNEL = 0x02;

	QByteArray inputData;
	inputData.append(static_cast<char>(mPinId));

	inputData.append(mChat.size());
	inputData.append(mChat);

	inputData.append(static_cast<char>(0));
	inputData.append((mCertificateDescription.size() >> 0) & 0xff);
	inputData.append((mCertificateDescription.size() >> 8) & 0xff);
	inputData.append(mCertificateDescription);

	QByteArray commandData;
	commandData.append(INDEX_ESTABLISH_PACE_CHANNEL);
	commandData.append((inputData.size() >> 0) & 0xff);
	commandData.append((inputData.size() >> 8) & 0xff);
	commandData.append(inputData);
	return commandData;
}


CommandApdu EstablishPACEChannelBuilder::createCommandDataCcid()
{
	auto channelInput = newObject<ESTABLISHPACECHANNELINPUT>();

	ASN1_INTEGER_set(channelInput->mPasswordID, static_cast<long>(mPinId));
	if (!mChat.isNull())
	{
		channelInput->mCHAT = ASN1_OCTET_STRING_new();
		Asn1OctetStringUtil::setValue(mChat, channelInput->mCHAT);
	}
	if (!mCertificateDescription.isEmpty())
	{
		const unsigned char* unsignedCharPointer = reinterpret_cast<unsigned char*>(mCertificateDescription.data());
		decodeAsn1Object(&channelInput->mCertificateDescription, &unsignedCharPointer, mCertificateDescription.size());
	}

	QByteArray data = encodeObject(channelInput.data());

	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x02, data, CommandApdu::SHORT_MAX_LE);
}


EstablishPACEChannelOutput::EstablishPACEChannelOutput()
	: mPaceReturnCode(ReturnCode::UNKNOWN)
	, mEfCardAccess()
	, mCarCurr()
	, mCarPrev()
	, mIdIcc()
{
}


ReturnCode EstablishPACEChannelOutput::getPaceReturnCode() const
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


void EstablishPACEChannelOutput::setPaceReturnCode(ReturnCode pPaceReturnCode)
{
	mPaceReturnCode = pPaceReturnCode;
}


uint EstablishPACEChannelOutput::parseUSHORT(const QByteArray& pData, int pOffset)
{
	uint len = static_cast<uchar>(pData.at(pOffset));
	len += (static_cast<uchar>(pData.at(pOffset + 1)) << 8);
	return len;
}


QByteArray EstablishPACEChannelOutput::reverse(const QByteArray& pArrayToReverse)
{
	QByteArray reversed;
	for (int i = pArrayToReverse.size() - 1; i >= 0; i--)
	{
		reversed.append(pArrayToReverse.at(i));
	}
	return reversed;
}


void EstablishPACEChannelOutput::parse(const QByteArray& pControlOutput, PACE_PIN_ID pPinId)
{
	if (pControlOutput.size() < 6)
	{
		qCWarning(card) << "Output of EstablishPACEChannel has wrong size";
		return;
	}
	quint32 paceReturnCode;
	QDataStream(reverse(pControlOutput.mid(0, 4))) >> paceReturnCode;
	mPaceReturnCode = parseReturnCode(paceReturnCode, pPinId);
	if (mPaceReturnCode == ReturnCode::UNKNOWN)
	{
		mPaceReturnCode = PersoSimWorkaround::parsingEstablishPACEChannelOutput(pControlOutput, pPinId);
	}

	uint dataLength = parseUSHORT(pControlOutput.mid(4, 2), 0);
	if (static_cast<uint>(pControlOutput.size()) < 6 + dataLength)
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
	int it = 6;
	//uint status = (static_cast<uchar>(pControlOutput.at(it)) << 8);
	++it;
	//status += static_cast<uchar>(pControlOutput.at(it));
	++it;

	uint lengthCardAccess = parseUSHORT(pControlOutput, it);
	it += 2;

	mEfCardAccess.append(pControlOutput.mid(it, lengthCardAccess));
	it += lengthCardAccess;

	if (it >= pControlOutput.size())
	{
		// in case of managing eSign PIN no CAR or IdICC is contained
		qCDebug(card) << "No CAR or IdICC contained";
		return;
	}

	uint length_CARcurr = static_cast<uchar>(pControlOutput.at(it));
	++it;
	mCarCurr.append(pControlOutput.mid(it, length_CARcurr));
	it += length_CARcurr;
	qCDebug(card) << "mCarCurr" << mCarCurr;

	uint length_CARprev = static_cast<uchar>(pControlOutput.at(it));
	++it;
	mCarPrev.append(pControlOutput.mid(it, length_CARprev));
	it += length_CARprev;
	qCDebug(card) << "mCarPrev" << mCarPrev;

	uint length_IDicc = parseUSHORT(pControlOutput, it);
	it += 2;
	mIdIcc.append(pControlOutput.mid(it, length_IDicc));
}


void EstablishPACEChannelOutput::parseFromCcid(const QByteArray& pOutput, PACE_PIN_ID pPinId)
{
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
			QByteArray paceReturnCodeBytes = QByteArray::fromHex(match.captured("a1").toUtf8());
			quint32 paceReturnCode;
			QDataStream(paceReturnCodeBytes) >> paceReturnCode;
			mPaceReturnCode = parseReturnCode(paceReturnCode, pPinId);
			qCDebug(card) << "mPaceReturnCode: " << mPaceReturnCode << paceReturnCodeBytes.toHex();
		}
		return;
	}

	QByteArray paceReturnCodeBytes = Asn1OctetStringUtil::getValue(channelOutput->mErrorCode);
	quint32 paceReturnCode;
	QDataStream(paceReturnCodeBytes) >> paceReturnCode;
	mPaceReturnCode = parseReturnCode(paceReturnCode, pPinId);
	qDebug() << "mPaceReturnCode: " << mPaceReturnCode << paceReturnCodeBytes.toHex();

	auto statusMseSetAT = Asn1OctetStringUtil::getValue(channelOutput->mStatusMSESetAt);
	qDebug() << "statusMSESetAT: " << statusMseSetAT.toHex();

	mEfCardAccess = encodeObject(channelOutput->mEfCardAccess);
	qDebug() << "mEfCardAccess" << mEfCardAccess.toHex();

	if (channelOutput->mIdPICC != nullptr)
	{
		mIdIcc = Asn1OctetStringUtil::getValue(channelOutput->mIdPICC);
		qDebug() << "idicc: " << mIdIcc.toHex();
	}

	if (channelOutput->mCurCAR != nullptr)
	{
		mCarCurr = Asn1OctetStringUtil::getValue(channelOutput->mCurCAR);
		qDebug() << "mCarCurr" << mCarCurr;
	}

	if (channelOutput->mPrevCAR != nullptr)
	{
		mCarPrev = Asn1OctetStringUtil::getValue(channelOutput->mPrevCAR);
		qDebug() << "mCarPrev" << mCarPrev;
	}
}


ReturnCode EstablishPACEChannelOutput::parseReturnCode(quint32 pPaceReturnCode, PACE_PIN_ID pPinId)
{
	// error codes from the reader
	switch (pPaceReturnCode)
	{
		case 0:
			// no error
			return ReturnCode::OK;

		case 0xd0000001: // Inconsistent lengths in input
		case 0xd0000002: // Unexpected data in input
		case 0xd0000003: // Unexpected combination of data in input
		case 0xe0000001: // Syntax error in TLV response
		case 0xe0000002: // Unexpected or missing object in TLV response
		case 0xe0000003: // Unknown PIN-ID
		case 0xe0000006: // Wrong Authentication Token
			return ReturnCode::COMMAND_FAILED;

		// 0xf00663c2 -- invalid PIN?
		case 0xf0100001: // Communication abort (e.g. card removed during protocol)
		case 0xf0100002: // No card
			return ReturnCode::COMMAND_FAILED;

		case 0xf0200001: // Abort
			return ReturnCode::CANCELLATION_BY_USER;

		case 0xf0200002: // Timeout
			return ReturnCode::TIME_OUT;
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
				switch (pPinId)
				{
					case PACE_PIN_ID::PACE_MRZ:
					// No separate error code (yet).
					case PACE_PIN_ID::PACE_CAN:
						return ReturnCode::CAN_INVALID;

					case PACE_PIN_ID::PACE_PIN:
						return ReturnCode::PIN_INVALID;

					case PACE_PIN_ID::PACE_PUK:
						return ReturnCode::PUK_INVALID;
				}
			}
			break;
	}

	return ReturnCode::UNKNOWN;
}
