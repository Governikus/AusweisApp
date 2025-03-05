/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "apdu/PacePinStatus.h"

#include <QtTest>

using namespace governikus;

class test_PacePinStatus
	: public QObject
{
	Q_OBJECT

	private:
		bool mTestAllKnownStatusCode;
		QList<StatusCode> mStatusCodeToTest;

	private Q_SLOTS:
		void cleanupTestCase()
		{
			QVERIFY(!mTestAllKnownStatusCode || mStatusCodeToTest.isEmpty());
		}


		void test_knownStatusCodes_data()
		{
			QTest::addColumn<StatusCode>("statusCode");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<bool>("pinInitial");
			QTest::addColumn<bool>("pinDeactivated");

			QTest::newRow("UNKNOWN") << StatusCode::UNKNOWN << -1 << false << false;
			QTest::newRow("SUCCESS") << StatusCode::SUCCESS << 3 << false << false;
			QTest::newRow("NO_PKCS15_APP") << StatusCode::NO_PKCS15_APP << -1 << false << false;
			QTest::newRow("END_OF_FILE") << StatusCode::END_OF_FILE << -1 << false << false;
			QTest::newRow("PIN_DEACTIVATED") << StatusCode::PIN_DEACTIVATED << 0 << false << true;
			QTest::newRow("FCI_NO_ISO7816_4") << StatusCode::FCI_NO_ISO7816_4 << -1 << false << false;
			QTest::newRow("VERIFICATION_FAILED") << StatusCode::VERIFICATION_FAILED << -1 << false << false;
			QTest::newRow("INPUT_TIMEOUT") << StatusCode::INPUT_TIMEOUT << -1 << false << false;
			QTest::newRow("INPUT_CANCELLED") << StatusCode::INPUT_CANCELLED << -1 << false << false;
			QTest::newRow("PASSWORDS_DIFFER") << StatusCode::PASSWORDS_DIFFER << -1 << false << false;
			QTest::newRow("PASSWORD_OUTOF_RANGE") << StatusCode::PASSWORD_OUTOF_RANGE << -1 << false << false;
			QTest::newRow("CARD_EJECTED_AND_REINSERTED") << StatusCode::CARD_EJECTED_AND_REINSERTED << -1 << false << false;
			QTest::newRow("EEPROM_CELL_DEFECT") << StatusCode::EEPROM_CELL_DEFECT << -1 << false << false;
			QTest::newRow("SECURITY_ENVIRONMENT") << StatusCode::SECURITY_ENVIRONMENT << -1 << false << false;
			QTest::newRow("WRONG_LENGTH") << StatusCode::WRONG_LENGTH << -1 << false << false;
			QTest::newRow("NO_BINARY_FILE") << StatusCode::NO_BINARY_FILE << -1 << false << false;
			QTest::newRow("LAST_CHAIN_CMD_EXPECTED") << StatusCode::LAST_CHAIN_CMD_EXPECTED << -1 << false << false;
			QTest::newRow("ACCESS_DENIED") << StatusCode::ACCESS_DENIED << -1 << false << false;
			QTest::newRow("PASSWORD_COUNTER_EXPIRED") << StatusCode::PASSWORD_COUNTER_EXPIRED << -1 << false << false;
			QTest::newRow("DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED") << StatusCode::DIRECTORY_OR_PASSWORD_LOCKED_OR_NOT_ALLOWED << -1 << false << false;
			QTest::newRow("NO_PARENT_FILE") << StatusCode::NO_PARENT_FILE << -1 << false << false;
			QTest::newRow("NOT_YET_INITIALIZED") << StatusCode::NOT_YET_INITIALIZED << -1 << false << false;
			QTest::newRow("NO_CURRENT_DIRECTORY_SELECTED") << StatusCode::NO_CURRENT_DIRECTORY_SELECTED << -1 << false << false;
			QTest::newRow("DATAFIELD_EXPECTED") << StatusCode::DATAFIELD_EXPECTED << -1 << false << false;
			QTest::newRow("INVALID_SM_OBJECTS") << StatusCode::INVALID_SM_OBJECTS << -1 << false << false;
			QTest::newRow("SW_APPLET_SELECT_FAILED") << StatusCode::SW_APPLET_SELECT_FAILED << -1 << false << false;
			QTest::newRow("COMMAND_NOT_ALLOWED") << StatusCode::COMMAND_NOT_ALLOWED << -1 << false << false;
			QTest::newRow("INVALID_DATAFIELD") << StatusCode::INVALID_DATAFIELD << -1 << false << false;
			QTest::newRow("ALGORITHM_ID") << StatusCode::ALGORITHM_ID << -1 << false << false;
			QTest::newRow("FILE_NOT_FOUND") << StatusCode::FILE_NOT_FOUND << -1 << false << false;
			QTest::newRow("RECORD_NOT_FOUND") << StatusCode::RECORD_NOT_FOUND << -1 << false << false;
			QTest::newRow("INVALID_PARAMETER") << StatusCode::INVALID_PARAMETER << -1 << false << false;
			QTest::newRow("LC_INCONSISTANT") << StatusCode::LC_INCONSISTANT << -1 << false << false;
			QTest::newRow("REFERENCED_DATA_NOT_FOUND") << StatusCode::REFERENCED_DATA_NOT_FOUND << -1 << false << false;
			QTest::newRow("ILLEGAL_OFFSET") << StatusCode::ILLEGAL_OFFSET << -1 << false << false;
			QTest::newRow("UNSUPPORTED_CLA") << StatusCode::UNSUPPORTED_CLA << -1 << false << false;
			QTest::newRow("CANT_DISPLAY") << StatusCode::CANT_DISPLAY << -1 << false << false;
			QTest::newRow("INVALID_P1P2") << StatusCode::INVALID_P1P2 << -1 << false << false;
			QTest::newRow("UNSUPPORTED_INS") << StatusCode::UNSUPPORTED_INS << -1 << false << false;
			QTest::newRow("PIN_BLOCKED") << StatusCode::PIN_BLOCKED << 0 << false << false;
			QTest::newRow("PIN_SUSPENDED") << StatusCode::PIN_SUSPENDED << 1 << false << false;
			QTest::newRow("PIN_RETRY_COUNT_2") << StatusCode::PIN_RETRY_COUNT_2 << 2 << false << false;
			QTest::newRow("INITIAL_PIN_BLOCKED") << StatusCode::INITIAL_PIN_BLOCKED << 0 << true << false;
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_1") << StatusCode::INITIAL_PIN_RETRY_COUNT_1 << 1 << true << false;
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_2") << StatusCode::INITIAL_PIN_RETRY_COUNT_2 << 2 << true << false;
			QTest::newRow("INITIAL_PIN_RETRY_COUNT_3") << StatusCode::INITIAL_PIN_RETRY_COUNT_3 << 3 << true << false;
			QTest::newRow("NO_PRECISE_DIAGNOSIS") << StatusCode::NO_PRECISE_DIAGNOSIS << -1 << false << false;

			mTestAllKnownStatusCode = true;
		}


		void test_knownStatusCodes()
		{
			QFETCH(StatusCode, statusCode);
			QFETCH(int, retryCounter);
			QFETCH(bool, pinInitial);
			QFETCH(bool, pinDeactivated);

			QCOMPARE(PacePinStatus::getRetryCounter(statusCode), retryCounter);
			QCOMPARE(PacePinStatus::isInitial(statusCode), pinInitial);
			QCOMPARE(PacePinStatus::isDeactivated(statusCode), pinDeactivated);

			mStatusCodeToTest.removeOne(statusCode);
		}

	public:
		test_PacePinStatus()
			: mTestAllKnownStatusCode(false)
			, mStatusCodeToTest(Enum<StatusCode>::getList())
		{
		}


};

QTEST_GUILESS_MAIN(test_PacePinStatus)
#include "test_PacePinStatus.moc"
