/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "pinpad/PinModify.h"


using namespace governikus;


class test_PinModify
	: public QObject
{
	Q_OBJECT

	public:
		test_PinModify()
			: mTimeoutSeconds(5)
		{
		}

	private:
		const quint8 mTimeoutSeconds;

	private Q_SLOTS:
		void createCcid()
		{
			quint8 timeout = 77;
			PinModify pinModify(timeout);

			QByteArray expected;
			expected += static_cast<char>(timeout);
			expected += static_cast<char>(timeout);
			expected += QByteArray::fromHex("82000000000606010202070400010200000004000000002C0203");

			QCOMPARE(pinModify.createCcid(), expected);
		}


		void parseSampleData()
		{
			const QByteArray abPINDataStructure = QByteArray::fromHex(
					"05" // bTimeOut (timeout in seconds)
					"06" // bTimeOut2 (timeout in seconds after first key stroke)
					"82" // bmFormatString
					"00" // bmPINBlockString
					"00" // bmPINLengthFormat
					"00" // bInsertionOffsetOld
					"00" // bInsertionOffsetNew
					"0606" // wPINMaxExtraDigit USHORT
					"01" // bConfirmPIN
					"02" // bEntryValidationCondition
					"02" // bNumberMessage
					"0704" // wLangId: German (0x0407)
					"01" // bMsgIndex1
					"02" // bMsgIndex2
					"00" // bMsgIndex3
					"000000" // bTeoPrologue
					"04" // ulDataLength
					"00" // CLA: command
					"2c" // INS: Reset Retry Counter
					"02" // P1: new PIN/CAN
					"03"); // P2: PIN

			PinModify pinModify(abPINDataStructure);
			QCOMPARE(pinModify.getTimeoutSeconds(), mTimeoutSeconds);
		}


		void buildAndParse()
		{
			PinModify builder(mTimeoutSeconds);
			const QByteArray data = builder.createCcid();

			PinModify parser(data);
			QCOMPARE(parser.getTimeoutSeconds(), mTimeoutSeconds);
		}


};

QTEST_GUILESS_MAIN(test_PinModify)
#include "test_PinModify.moc"
