/*!
 * \brief Unit tests for \ref Apdu
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ResponseApdu.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_ResponceApdu
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void testRetryCounter_data()
		{
			QTest::addColumn<QByteArray>("apdu");
			QTest::addColumn<int>("retryCounter");

			QTest::newRow("9000") << QByteArray::fromHex("9000") << 3;
			QTest::newRow("63c2") << QByteArray::fromHex("63c2") << 2;
			QTest::newRow("63c1") << QByteArray::fromHex("63c1") << 1;
			QTest::newRow("63c0") << QByteArray::fromHex("63c0") << 0;
			QTest::newRow("63c3") << QByteArray::fromHex("63c3") << -1;
			QTest::newRow("6400") << QByteArray::fromHex("6400") << -1;
			QTest::newRow("1234") << QByteArray::fromHex("1234") << -1;
			QTest::newRow("12") << QByteArray::fromHex("12") << -1;
			QTest::newRow("6283") << QByteArray::fromHex("6283") << 0;
			QTest::newRow("empty") << QByteArray() << -1;
		}


		void testRetryCounter()
		{
			QFETCH(QByteArray, apdu);
			QFETCH(int, retryCounter);

			QCOMPARE(ResponseApdu(apdu).getRetryCounter(), retryCounter);
		}


		void testReturnCode_data()
		{
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<QByteArray>("bufferIn");
			QTest::addColumn<QByteArray>("bufferOut");

			QTest::newRow("empty") << StatusCode::EMPTY << QByteArray() << QByteArray::fromHex("0000");
			QTest::newRow("01") << StatusCode::INVALID << QByteArray::fromHex("01") << QByteArray::fromHex("0001");
			QTest::newRow("63c2") << StatusCode::PIN_RETRY_COUNT_2 << QByteArray::fromHex("63c2") << QByteArray::fromHex("63c2");
			QTest::newRow("6401") << StatusCode::INPUT_CANCELLED << QByteArray::fromHex("6401") << QByteArray::fromHex("6401");
			QTest::newRow("73c2") << StatusCode::INVALID << QByteArray::fromHex("73c2") << QByteArray::fromHex("0001");
		}


		void testReturnCode()
		{
			QFETCH(StatusCode, statusCode);
			QFETCH(QByteArray, bufferIn);
			QFETCH(QByteArray, bufferOut);

			ResponseApdu apdu = ResponseApdu(statusCode);
			QCOMPARE(apdu.getReturnCode(), statusCode);
			QCOMPARE(apdu.getBuffer(), bufferOut);

			apdu = ResponseApdu(bufferIn);
			QCOMPARE(apdu.getReturnCode(), statusCode);
			QCOMPARE(apdu.getBuffer(), bufferIn);
		}


		void testCardReturnCode_data()
		{
			QTest::addColumn<CardReturnCode>("cardReturnCode");
			QTest::addColumn<StatusCode>("statCode");

			QTest::newRow("SUCCESS") << CardReturnCode::OK << StatusCode::SUCCESS;
			QTest::newRow("INPUT_TIMEOUT") << CardReturnCode::INPUT_TIME_OUT << StatusCode::INPUT_TIMEOUT;
			QTest::newRow("INPUT_CANCELLED") << CardReturnCode::CANCELLATION_BY_USER << StatusCode::INPUT_CANCELLED;
			QTest::newRow("PASSWORDS_DIFFER") << CardReturnCode::NEW_PIN_MISMATCH << StatusCode::PASSWORDS_DIFFER;
			QTest::newRow("PASSWORD_OUTOF_RANGE") << CardReturnCode::NEW_PIN_INVALID_LENGTH << StatusCode::PASSWORD_OUTOF_RANGE;
			QTest::newRow("PIN_BLOCKED") << CardReturnCode::PIN_BLOCKED << StatusCode::PIN_BLOCKED;
			QTest::newRow("default") << CardReturnCode::PROTOCOL_ERROR << StatusCode::NOT_YET_INITIALIZED;
		}


		void testCardReturnCode()
		{
			QFETCH(CardReturnCode, cardReturnCode);
			QFETCH(StatusCode, statCode);

			ResponseApdu apdu = ResponseApdu(statCode);
			QCOMPARE(apdu.getCardReturnCode(), cardReturnCode);
		}


		void test_getSW2()
		{
			ResponseApdu apdu1 = ResponseApdu(QByteArray());
			ResponseApdu apdu2 = ResponseApdu(QByteArray("a"));
			ResponseApdu apdu3 = ResponseApdu(QByteArray("date"));
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QCOMPARE(apdu1.getSW2(), '\0');
			QCOMPARE(apdu2.getSW2(), 'a');
			QCOMPARE(apdu3.getSW2(), 'e');
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.at(0).at(0).toString().contains("Buffer too short, returning 0"));
		}


		void testGetSW1_data()
		{
			QTest::addColumn<SW1>("sw1");
			QTest::addColumn<QByteArray>("buffer");
			QTest::addColumn<QString>("warning");

			QTest::newRow("empty") << SW1::INVALID << QByteArray() << QString("Buffer too short, returning 0");
			QTest::newRow("0000") << SW1::INVALID << QByteArray::fromHex("0000") << QString();
			QTest::newRow("6100") << SW1::MORE_DATA_AVAILABLE << QByteArray::fromHex("6100") << QString();
			QTest::newRow("6900") << SW1::ERROR_COMMAND_NOT_ALLOWED << QByteArray::fromHex("6900") << QString();
			QTest::newRow("6c00") << SW1::WRONG_LE_FIELD << QByteArray::fromHex("6c00") << QString();
			QTest::newRow("invalid") << SW1::INVALID << QByteArray::fromHex("3300") << QString("Unknown SW1 value, returning INVALID, value: \"33\"");
		}


		void testGetSW1()
		{
			QFETCH(SW1, sw1);
			QFETCH(QByteArray, buffer);
			QFETCH(QString, warning);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			ResponseApdu apdu = ResponseApdu(buffer);
			QCOMPARE(apdu.getSW1(), sw1);
			if (!warning.isEmpty())
			{
				QCOMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.takeFirst().at(0).toString().contains(warning));
			}
		}


};


QTEST_GUILESS_MAIN(test_ResponceApdu)
#include "test_ResponseApdu.moc"
