/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "PinModify.h"

#include <QLoggingCategory>
#include <QtEndian>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


PinModify::PinModify(quint8 pTimeoutSeconds)
	: mTimeoutSeconds(pTimeoutSeconds)
{
}


PinModify::PinModify(const QByteArray& pRemoteInputData)
	: PinModify(0)
{
	if (pRemoteInputData.isEmpty())
	{
		qCDebug(card) << "Parsing failed. Command data should contain at least 1 bytes";
		return;
	}

	mTimeoutSeconds = static_cast<quint8>(pRemoteInputData.at(0));
}


QByteArray PinModify::createCcid() const
{
	// The documentation is available in:
	//     PC/SC, Part 10 "IFDs with Secure PIN Entry Capabilities", 2.5.3 "PIN_MODIFY"
	//     DWG_Smart-Card_CCID_Rev110.pdf as mentioned in [TR-03119]

	QByteArray abPINDataStructure;
	// bTimeOut (timeout in seconds)
	abPINDataStructure += static_cast<char>(mTimeoutSeconds);
	// bTimeOut2 (timeout in seconds)
	abPINDataStructure += static_cast<char>(mTimeoutSeconds);
	// bmFormatString (PIN format): system unit is bytes (0x80), ASCII format (0x02)
	abPINDataStructure += static_cast<char>(0x82);
	// bmPINBlockString (PIN block size and length info): PIN not in APDU command
	abPINDataStructure += '\0';
	// bmPINLengthFormat (format of PIN length field in APDU command): PIN not in APDU command
	abPINDataStructure += '\0';
	// bInsertionOffsetOld (insertion position offset for old PIN)
	abPINDataStructure += '\0';
	// bInsertionOffsetNew BYTE (insertion position offset for new PIN)
	abPINDataStructure += '\0';
	// wPINMaxExtraDigit USHORT (0xXXYY, min (XX) and max (length) of new PIN)
	abPINDataStructure += static_cast<char>(0x06);
	abPINDataStructure += static_cast<char>(0x06);
	// bConfirmPIN (PIN confirmation options): confirm new PIN (0x01)
	abPINDataStructure += static_cast<char>(0x01);
	// bEntryValidationCondition (new PIN validation options): validation key pressed (0x02)
	abPINDataStructure += static_cast<char>(0x02);
	// bNumberMessage (number of display messages to be sent)
	abPINDataStructure += static_cast<char>(0x02);
	// wLangId (language ID for display messages): German (0x0407)
	abPINDataStructure += static_cast<char>(0x07);
	abPINDataStructure += static_cast<char>(0x04);
	// bMsgIndex1 (index (into reader table) of first message to display)
	abPINDataStructure += static_cast<char>(0x00);
	// bMsgIndex2 (index (into reader table) of second message to display)
	abPINDataStructure += static_cast<char>(0x01);
	// bMsgIndex3 (index (into reader table) of third message to display)
	// WARNING: bMsgIndex3 should not be present for CCID because bNumberMessage != 3)
	//          We still need it, because Reiner SCT standard/comfort reader are not working without.
	abPINDataStructure += static_cast<char>(0x02);
	// bTeoPrologue (T1 only: I-block prologue field to use): fill with 0
	abPINDataStructure += '\0';
	abPINDataStructure += '\0';
	abPINDataStructure += '\0';

	// According to ISO-7816-4, 11.6.10 RESET RETRY COUNTER command
	const QByteArray abData = QByteArray::fromHex(QByteArrayLiteral("002C0203"));
	QByteArray ulDataLength(4, 0);
	qToLittleEndian<quint32>(static_cast<quint32>(abData.size()), ulDataLength.data());
	abPINDataStructure += ulDataLength;
	// PCSC: abData / CCID: abPINApdu
	abPINDataStructure += abData;

	return abPINDataStructure;
}


quint8 PinModify::getTimeoutSeconds() const
{
	return mTimeoutSeconds;
}
