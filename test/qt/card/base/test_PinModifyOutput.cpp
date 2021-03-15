/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>


#include "PinModifyOutput.h"


using namespace governikus;


class test_PinModifyOutput
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void fromCcidResponse()
		{
			PinModifyOutput output;

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("02")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::UNKNOWN);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("02"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6400")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::INPUT_TIME_OUT);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6400"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6401")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6401"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6402")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::NEW_PIN_MISMATCH);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6402"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6403")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::NEW_PIN_INVALID_LENGTH);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6403"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6a80")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6a80"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("6982")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::UNKNOWN);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("6982"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("9000")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::OK);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("9000"));

			output = PinModifyOutput(ResponseApdu(QByteArray::fromHex("beef")));
			QCOMPARE(output.getReturnCode(), CardReturnCode::UNKNOWN);
			QCOMPARE(output.getResponseApdu().getBuffer(), QByteArray::fromHex("beef"));
		}


		void toCcid()
		{
			QCOMPARE(PinModifyOutput(ResponseApdu(QByteArray::fromHex("9000"))).toCcid(), QByteArray::fromHex("9000"));
		}


};

QTEST_GUILESS_MAIN(test_PinModifyOutput)
#include "test_PinModifyOutput.moc"
