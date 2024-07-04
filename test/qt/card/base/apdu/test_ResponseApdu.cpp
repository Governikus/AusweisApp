/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref Apdu
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
		bool mTestAllKnownStatusCode = false;
		QList<StatusCode> mStatusCodeToTest = {};

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


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
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
			QCOMPARE(apduParseDefault.getSW1(), SW1::UNKNOWN);
			QCOMPARE(apduParseDefault.getSW2(), 0x00);
			QCOMPARE(QByteArray(apduParseDefault), QByteArray());

			ResponseApdu apduParse(data + QByteArray::fromHex("0000"));
			QCOMPARE(apduParse.isEmpty(), data.isEmpty());
			QCOMPARE(apduParse.getData(), data);
			QCOMPARE(apduParse.getStatusCode(), StatusCode::UNKNOWN);
			QCOMPARE(apduParse.getStatusBytes(), QByteArray::fromHex(0000));
			QCOMPARE(apduParse.getSW1(), SW1::UNKNOWN);
			QCOMPARE(apduParse.getSW2(), 0x00);
			QCOMPARE(QByteArray(apduParse), QByteArray());
		}


		void knownStatusCode_data()
		{
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<SW1>("sw1");
			QTest::addColumn<char>("sw2");

			QTest::newRow("SUCCESS") << StatusCode::SUCCESS << SW1::SUCCESS << static_cast<char>(0x00);
			QTest::newRow("NO_PKCS15_APP") << StatusCode::NO_PKCS15_APP << SW1::NONVOLATILE_MEMORY_UNCHANGED_1 << static_cast<char>(0x00);
			QTest::newRow("END_OF_FILE") << StatusCode::END_OF_FILE << SW1::NONVOLATILE_MEMORY_UNCHANGED_1 << static_cast<char>(0x82);
			QTest::newRow("PIN_DEACTIVATED") << StatusCode::PIN_DEACTIVATED << SW1::NONVOLATILE_MEMORY_UNCHANGED_1 << static_cast<char>(0x83);
			QTest::newRow("FCI_NO_ISO7816_4") << StatusCode::FCI_NO_ISO7816_4 << SW1::NONVOLATILE_MEMORY_UNCHANGED_1 << static_cast<char>(0x84);
			QTest::newRow("VERIFICATION_FAILED") << StatusCode::VERIFICATION_FAILED << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0x00);
			QTest::newRow("INPUT_TIMEOUT") << StatusCode::INPUT_TIMEOUT << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0x00);
			QTest::newRow("INPUT_CANCELLED") << StatusCode::INPUT_CANCELLED << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0x01);
			QTest::newRow("PASSWORDS_DIFFER") << StatusCode::PASSWORDS_DIFFER << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0x02);
			QTest::newRow("PASSWORD_OUTOF_RANGE") << StatusCode::PASSWORD_OUTOF_RANGE << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0x03);
			QTest::newRow("CARD_EJECTED_AND_REINSERTED") << StatusCode::CARD_EJECTED_AND_REINSERTED << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0xA2);
			QTest::newRow("EEPROM_CELL_DEFECT") << StatusCode::EEPROM_CELL_DEFECT << SW1::NONVOLATILE_MEMORY_CHANGED_2 << static_cast<char>(0x81);
			QTest::newRow("SECURITY_ENVIRONMENT") << StatusCode::SECURITY_ENVIRONMENT << SW1::SECURITY_ISSUE << static_cast<char>(0x00);
			QTest::newRow("WRONG_LENGTH") << StatusCode::WRONG_LENGTH << SW1::WRONG_LENGTH << static_cast<char>(0x00);
			QTest::newRow("NO_BINARY_FILE") << StatusCode::NO_BINARY_FILE << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x81);
			QTest::newRow("LAST_CHAIN_CMD_EXPECTED") << StatusCode::LAST_CHAIN_CMD_EXPECTED << SW1::FUNCTIONS_IN_CLASS_NOT_SUPPORTED << static_cast<char>(0x83);
			QTest::newRow("ACCESS_DENIED") << StatusCode::ACCESS_DENIED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x82);
			QTest::newRow("PASSWORD_COUNTER_EXPIRED") << StatusCode::PASSWORD_COUNTER_EXPIRED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x83);
			QTest::newRow("DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED") << StatusCode::DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x84);
			QTest::newRow("NO_PARENT_FILE") << StatusCode::NO_PARENT_FILE << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x85);
			QTest::newRow("NOT_YET_INITIALIZED") << StatusCode::NOT_YET_INITIALIZED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x85);
			QTest::newRow("NO_CURRENT_DIRECTORY_SELECTED") << StatusCode::NO_CURRENT_DIRECTORY_SELECTED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x86);
			QTest::newRow("DATAFIELD_EXPECTED") << StatusCode::DATAFIELD_EXPECTED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x87);
			QTest::newRow("INVALID_SM_OBJECTS") << StatusCode::INVALID_SM_OBJECTS << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x88);
			QTest::newRow("SW_APPLET_SELECT_FAILED") << StatusCode::SW_APPLET_SELECT_FAILED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0x99);
			QTest::newRow("COMMAND_NOT_ALLOWED") << StatusCode::COMMAND_NOT_ALLOWED << SW1::ERROR_COMMAND_NOT_ALLOWED << static_cast<char>(0xF0);
			QTest::newRow("INVALID_DATAFIELD") << StatusCode::INVALID_DATAFIELD << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x80);
			QTest::newRow("ALGORITHM_ID") << StatusCode::ALGORITHM_ID << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x81);
			QTest::newRow("FILE_NOT_FOUND") << StatusCode::FILE_NOT_FOUND << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x82);
			QTest::newRow("RECORD_NOT_FOUND") << StatusCode::RECORD_NOT_FOUND << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x83);
			QTest::newRow("INVALID_PARAMETER") << StatusCode::INVALID_PARAMETER << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x86);
			QTest::newRow("LC_INCONSISTANT") << StatusCode::LC_INCONSISTANT << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x87);
			QTest::newRow("PASSWORD_NOT_FOUND") << StatusCode::PASSWORD_NOT_FOUND << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x88);
			QTest::newRow("ILLEGAL_OFFSET") << StatusCode::ILLEGAL_OFFSET << SW1::WRONG_PARAMETERS_P1_P2_NO_INFO << static_cast<char>(0x00);
			QTest::newRow("UNSUPPORTED_CLA") << StatusCode::UNSUPPORTED_CLA << SW1::CLASS_NOT_SUPPORTED << static_cast<char>(0x00);
			QTest::newRow("CANT_DISPLAY") << StatusCode::CANT_DISPLAY << SW1::NONVOLATILE_MEMORY_UNCHANGED_2 << static_cast<char>(0x10);
			QTest::newRow("INVALID_P1P2") << StatusCode::INVALID_P1P2 << SW1::WRONG_PARAMETERS_P1_P2 << static_cast<char>(0x00);
			QTest::newRow("UNSUPPORTED_INS") << StatusCode::UNSUPPORTED_INS << SW1::INSTRUCTION_CODE_INVALID << static_cast<char>(0x00);
			QTest::newRow("PIN_BLOCKED") << StatusCode::PIN_BLOCKED << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xC0);
			QTest::newRow("PIN_SUSPENDED") << StatusCode::PIN_SUSPENDED << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xC1);
			QTest::newRow("PIN_RETRY_COUNT_2") << StatusCode::PIN_RETRY_COUNT_2 << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xC2);
			QTest::newRow("INITIAL_PIN_BLOCKED") << StatusCode::INITIAL_PIN_BLOCKED << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xD0);
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_1") << StatusCode::INITIAL_PIN_RETRY_COUNT_1 << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xD1);
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_2") << StatusCode::INITIAL_PIN_RETRY_COUNT_2 << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xD2);
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_3") << StatusCode::INITIAL_PIN_RETRY_COUNT_3 << SW1::NONVOLATILE_MEMORY_CHANGED_1 << static_cast<char>(0xD3);

			mTestAllKnownStatusCode = true;
		}


		void knownStatusCode()
		{
			QFETCH_GLOBAL(QByteArray, data);
			QFETCH(StatusCode, statusCode);
			QFETCH(SW1, sw1);
			QFETCH(char, sw2);

			QByteArray statusBytes;
			statusBytes += static_cast<char>(sw1);
			statusBytes += sw2;
			QByteArray buffer = data + statusBytes;

			ResponseApdu apduCreate(statusCode, data);
			QVERIFY(!apduCreate.isEmpty());
			QCOMPARE(apduCreate.getData(), data);
			QCOMPARE(apduCreate.getStatusCode(), statusCode);
			QCOMPARE(apduCreate.getStatusBytes(), statusBytes);
			QCOMPARE(apduCreate.getSW1(), sw1);
			QCOMPARE(apduCreate.getSW2(), sw2);
			QCOMPARE(QByteArray(apduCreate), buffer);

			ResponseApdu apduParse(buffer);
			QVERIFY(!apduParse.isEmpty());
			QCOMPARE(apduParse.getData(), data);
			QCOMPARE(apduParse.getStatusCode(), statusCode);
			QCOMPARE(apduParse.getStatusBytes(), statusBytes);
			QCOMPARE(apduParse.getSW1(), sw1);
			QCOMPARE(apduParse.getSW2(), sw2);
			QCOMPARE(QByteArray(apduParse), buffer);

			mStatusCodeToTest.removeOne(statusCode);
		}


		void unknownStatusCode_data()
		{
			QTest::addColumn<QByteArray>("statusBytes");
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<SW1>("sw1");
			QTest::addColumn<char>("sw2");

			QTest::newRow("01") << QByteArray::fromHex("01") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0x01);
			QTest::newRow("62") << QByteArray::fromHex("62") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0x62);
			QTest::newRow("65") << QByteArray::fromHex("65") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0x65);
			QTest::newRow("90") << QByteArray::fromHex("90") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0x90);
			QTest::newRow("ab") << QByteArray::fromHex("AB") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0xAB);
			QTest::newRow("0090") << QByteArray::fromHex("0090") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0x90);
			QTest::newRow("6201") << QByteArray::fromHex("6201") << StatusCode::UNKNOWN << SW1::NONVOLATILE_MEMORY_UNCHANGED_1 << static_cast<char>(0x01);
			QTest::newRow("abcd") << QByteArray::fromHex("ABCD") << StatusCode::UNKNOWN << SW1::UNKNOWN << static_cast<char>(0xCD);
		}


		void unknownStatusCode()
		{
			QFETCH(QByteArray, statusBytes);
			QFETCH(StatusCode, statusCode);
			QFETCH(SW1, sw1);
			QFETCH(char, sw2);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			ResponseApdu apduParse(statusBytes);
			if (statusBytes.size() == 1)
			{
				QCOMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.takeFirst().at(0).toString().contains(QStringLiteral("One byte status, assuming")));
			}

			QByteArray resultStatusBytes = statusBytes;
			if (resultStatusBytes.size() < 2)
			{
				resultStatusBytes.prepend(static_cast<char>(0x00));
			}

			QVERIFY(!apduParse.isEmpty());
			QCOMPARE(apduParse.getData(), QByteArray());
			QCOMPARE(apduParse.getStatusCode(), statusCode);
			if (statusCode == StatusCode::UNKNOWN)
			{
				QCOMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.takeFirst().at(0).toString().contains(QStringLiteral("Unknown StatusCode value, returning UNKNOWN, value:")));
			}
			else
			{
				QVERIFY(logSpy.isEmpty());
			}
			QCOMPARE(apduParse.getStatusBytes(), resultStatusBytes);
			QCOMPARE(apduParse.getSW1(), sw1);
			if (sw1 == SW1::UNKNOWN)
			{
				QCOMPARE(logSpy.count(), 1);
				QVERIFY(logSpy.takeFirst().at(0).toString().contains(QStringLiteral("Unknown SW1 value, returning UNKNOWN, value:")));
			}
			else
			{
				QVERIFY(logSpy.isEmpty());

			}
			QCOMPARE(apduParse.getSW2(), sw2);
			QCOMPARE(QByteArray(apduParse), resultStatusBytes);

		}


		void test_logging()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			qDebug() << ResponseApdu(StatusCode::ACCESS_DENIED);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("6982"_L1));

			qDebug() << ResponseApdu(StatusCode::SUCCESS, QByteArray::fromHex("010203040506070809"));
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("\"0102030405~9000\" (11)"_L1));
		}


};


QTEST_GUILESS_MAIN(test_ResponseApdu)
#include "test_ResponseApdu.moc"
