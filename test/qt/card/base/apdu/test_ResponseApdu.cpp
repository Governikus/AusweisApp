/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "apdu/ResponseApdu.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_ResponseApdu
	: public QObject
{
	Q_OBJECT

	private:
		QList<StatusCode> mStatusCodeToTest;
		bool mTestAllKnownStatusCode;

	private Q_SLOTS:
		void initTestCase_data()
		{
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("EmptyData") << QByteArray();
			QTest::newRow("1ByteData") << QByteArray("a");
			QTest::newRow("3ByteData") << QByteArray("abc");
		}


		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();

			mTestAllKnownStatusCode = false;
			mStatusCodeToTest = Enum<StatusCode>::getList();
			mStatusCodeToTest.removeOne(StatusCode::UNKNOWN);
			mStatusCodeToTest += (mStatusCodeToTest + mStatusCodeToTest);
		}


		void init()
		{
			QLoggingCategory::setFilterRules(QStringLiteral("secure.debug=true"));
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
			QCoreApplication::instance()->processEvents();
		}


		void cleanupTestCase()
		{
			QVERIFY(!mTestAllKnownStatusCode || mStatusCodeToTest.isEmpty());
		}


		void empty()
		{
			QFETCH_GLOBAL(QByteArray, data);

			ResponseApdu apduParseDefault;
			QVERIFY(apduParseDefault.isEmpty());
			QCOMPARE(apduParseDefault.getData(), QByteArray());
			QCOMPARE(apduParseDefault.getStatusCode(), StatusCode::UNKNOWN);
			QCOMPARE(apduParseDefault.getStatusBytes(), QByteArray::fromHex(0000));
			QCOMPARE(QByteArray(apduParseDefault), QByteArray());

			ResponseApdu apduParse(data + QByteArray::fromHex("0000"));
			QCOMPARE(apduParse.isEmpty(), data.isEmpty());
			QCOMPARE(apduParse.getData(), data);
			QCOMPARE(apduParse.getStatusCode(), StatusCode::UNKNOWN);
			QCOMPARE(apduParse.getStatusBytes(), QByteArray::fromHex(0000));
			QCOMPARE(QByteArray(apduParse), QByteArray());
		}


		void knownStatusCode_data()
		{
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<int>("retryCounter");

			QTest::newRow("SUCCESS") << StatusCode::SUCCESS << 3;
			QTest::newRow("NO_PKCS15_APP") << StatusCode::NO_PKCS15_APP << -1;
			QTest::newRow("END_OF_FILE") << StatusCode::END_OF_FILE << -1;
			QTest::newRow("PIN_DEACTIVATED") << StatusCode::PIN_DEACTIVATED << 0;
			QTest::newRow("FCI_NO_ISO7816_4") << StatusCode::FCI_NO_ISO7816_4 << -1;
			QTest::newRow("VERIFICATION_FAILED") << StatusCode::VERIFICATION_FAILED << -1;
			QTest::newRow("INPUT_TIMEOUT") << StatusCode::INPUT_TIMEOUT << -1;
			QTest::newRow("INPUT_CANCELLED") << StatusCode::INPUT_CANCELLED << -1;
			QTest::newRow("PASSWORDS_DIFFER") << StatusCode::PASSWORDS_DIFFER << -1;
			QTest::newRow("PASSWORD_OUTOF_RANGE") << StatusCode::PASSWORD_OUTOF_RANGE << -1;
			QTest::newRow("CARD_EJECTED_AND_REINSERTED") << StatusCode::CARD_EJECTED_AND_REINSERTED << -1;
			QTest::newRow("EEPROM_CELL_DEFECT") << StatusCode::EEPROM_CELL_DEFECT << -1;
			QTest::newRow("SECURITY_ENVIRONMENT") << StatusCode::SECURITY_ENVIRONMENT << -1;
			QTest::newRow("WRONG_LENGTH") << StatusCode::WRONG_LENGTH << -1;
			QTest::newRow("NO_BINARY_FILE") << StatusCode::NO_BINARY_FILE << -1;
			QTest::newRow("LAST_CHAIN_CMD_EXPECTED") << StatusCode::LAST_CHAIN_CMD_EXPECTED << -1;
			QTest::newRow("ACCESS_DENIED") << StatusCode::ACCESS_DENIED << -1;
			QTest::newRow("PASSWORD_COUNTER_EXPIRED") << StatusCode::PASSWORD_COUNTER_EXPIRED << -1;
			QTest::newRow("DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED") << StatusCode::DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED << -1;
			QTest::newRow("NO_PARENT_FILE") << StatusCode::NO_PARENT_FILE << -1;
			QTest::newRow("NOT_YET_INITIALIZED") << StatusCode::NOT_YET_INITIALIZED << -1;
			QTest::newRow("NO_CURRENT_DIRECTORY_SELECTED") << StatusCode::NO_CURRENT_DIRECTORY_SELECTED << -1;
			QTest::newRow("DATAFIELD_EXPECTED") << StatusCode::DATAFIELD_EXPECTED << -1;
			QTest::newRow("INVALID_SM_OBJECTS") << StatusCode::INVALID_SM_OBJECTS << -1;
			QTest::newRow("SW_APPLET_SELECT_FAILED") << StatusCode::SW_APPLET_SELECT_FAILED << -1;
			QTest::newRow("COMMAND_NOT_ALLOWED") << StatusCode::COMMAND_NOT_ALLOWED << -1;
			QTest::newRow("INVALID_DATAFIELD") << StatusCode::INVALID_DATAFIELD << -1;
			QTest::newRow("ALGORITHM_ID") << StatusCode::ALGORITHM_ID << -1;
			QTest::newRow("FILE_NOT_FOUND") << StatusCode::FILE_NOT_FOUND << -1;
			QTest::newRow("RECORD_NOT_FOUND") << StatusCode::RECORD_NOT_FOUND << -1;
			QTest::newRow("INVALID_PARAMETER") << StatusCode::INVALID_PARAMETER << -1;
			QTest::newRow("LC_INCONSISTENT") << StatusCode::LC_INCONSISTENT << -1;
			QTest::newRow("REFERENCED_DATA_NOT_FOUND") << StatusCode::REFERENCED_DATA_NOT_FOUND << -1;
			QTest::newRow("ILLEGAL_OFFSET") << StatusCode::ILLEGAL_OFFSET << -1;
			QTest::newRow("UNSUPPORTED_CLA") << StatusCode::UNSUPPORTED_CLA << -1;
			QTest::newRow("CANT_DISPLAY") << StatusCode::CANT_DISPLAY << -1;
			QTest::newRow("INVALID_P1P2") << StatusCode::INVALID_P1P2 << -1;
			QTest::newRow("UNSUPPORTED_INS") << StatusCode::UNSUPPORTED_INS << -1;
			QTest::newRow("PIN_BLOCKED") << StatusCode::PIN_BLOCKED << 0;
			QTest::newRow("PIN_SUSPENDED") << StatusCode::PIN_SUSPENDED << 1;
			QTest::newRow("PIN_RETRY_COUNT_2") << StatusCode::PIN_RETRY_COUNT_2 << 2;
			QTest::newRow("NO_PRECISE_DIAGNOSIS") << StatusCode::NO_PRECISE_DIAGNOSIS << -1;

			mTestAllKnownStatusCode = true;
		}


		void knownStatusCode()
		{
			QFETCH_GLOBAL(QByteArray, data);
			QFETCH(StatusCode, statusCode);
			QFETCH(int, retryCounter);

			QByteArray statusBytes(sizeof(quint16), 0);
			qToBigEndian(Enum<StatusCode>::getValue(statusCode), statusBytes.data());
			QByteArray buffer = data + statusBytes;

			ResponseApdu apduCreate(statusCode, data);
			QVERIFY(!apduCreate.isEmpty());
			QCOMPARE(apduCreate.getData(), data);
			QCOMPARE(apduCreate.getStatusCode(), statusCode);
			QCOMPARE(apduCreate.getStatusBytes(), statusBytes);
			QCOMPARE(apduCreate.getRetryCounter(), retryCounter);
			QCOMPARE(QByteArray(apduCreate), buffer);

			ResponseApdu apduParse(buffer);
			QVERIFY(!apduParse.isEmpty());
			QCOMPARE(apduParse.getData(), data);
			QCOMPARE(apduParse.getStatusCode(), statusCode);
			QCOMPARE(apduParse.getStatusBytes(), statusBytes);
			QCOMPARE(apduParse.getRetryCounter(), retryCounter);
			QCOMPARE(QByteArray(apduParse), buffer);

			mStatusCodeToTest.removeOne(statusCode);
		}


		void unknownStatusCode_data()
		{
			QTest::addColumn<QByteArray>("statusBytes");

			QTest::newRow("01") << QByteArray::fromHex("01");
			QTest::newRow("62") << QByteArray::fromHex("62");
			QTest::newRow("65") << QByteArray::fromHex("65");
			QTest::newRow("90") << QByteArray::fromHex("90");
			QTest::newRow("ab") << QByteArray::fromHex("AB");
			QTest::newRow("0090") << QByteArray::fromHex("0090");
			QTest::newRow("6201") << QByteArray::fromHex("6201");
			QTest::newRow("abcd") << QByteArray::fromHex("ABCD");
		}


		void unknownStatusCode()
		{
			QFETCH(QByteArray, statusBytes);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			ResponseApdu apduParse(statusBytes);
			if (statusBytes.size() == 1)
			{
				QTRY_COMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.takeFirst().at(0).toString().contains(QStringLiteral("One byte status, assuming")));
			}

			QByteArray resultStatusBytes = statusBytes;
			if (resultStatusBytes.size() < 2)
			{
				resultStatusBytes.prepend(static_cast<char>(0x00));
			}

			QVERIFY(!apduParse.isEmpty());
			QCOMPARE(apduParse.getData(), QByteArray());
			QCOMPARE(apduParse.getStatusCode(), StatusCode::UNKNOWN);
			QTRY_COMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains(QStringLiteral("Unknown StatusCode value, returning UNKNOWN, value:")));
			QCOMPARE(apduParse.getStatusBytes(), resultStatusBytes);
		}


		void test_logging_data()
		{
			QTest::addColumn<bool>("debug");
			QTest::addColumn<ResponseApdu>("input");
			QTest::addColumn<QLatin1String>("output");

			QTest::newRow("short private") << false << ResponseApdu(StatusCode::ACCESS_DENIED) << "6982"_L1;
			QTest::newRow("long private") << false << ResponseApdu(StatusCode::SUCCESS, QByteArray::fromHex("010203040506070809")) << "\"0102030405~9000\" (11)"_L1;

			QTest::newRow("short public") << true << ResponseApdu(StatusCode::ACCESS_DENIED) << "6982"_L1;
			QTest::newRow("long public") << true << ResponseApdu(StatusCode::SUCCESS, QByteArray::fromHex("010203040506070809")) << "0102030405060708099000"_L1;
		}


		void test_logging()
		{
			QFETCH(bool, debug);
			QFETCH(ResponseApdu, input);
			QFETCH(QLatin1String, output);

			QLoggingCategory::setFilterRules(QStringLiteral("secure.debug=%1").arg(QVariant(debug).toString()));

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			qDebug() << input;
			QTRY_COMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains(output));
		}


};


QTEST_GUILESS_MAIN(test_ResponseApdu)
#include "test_ResponseApdu.moc"
