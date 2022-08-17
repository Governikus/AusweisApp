/*!
 * \brief Unit tests for \ref SmartManager
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"
#include "SmartManager.h"

#include "mock/eid_applet_interface_mock.h"

#include <QSignalSpy>
#include <QTest>


using namespace governikus;


class test_SmartManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_get()
		{
			auto firstInstance = SmartManager::get();
			auto secondInstance = SmartManager::get();
			const auto& weakRef = firstInstance.toWeakRef();
			QVERIFY(firstInstance == secondInstance);
			QVERIFY(weakRef);
			firstInstance.reset();
			QVERIFY(weakRef);
			secondInstance.reset();
			QVERIFY(!weakRef);
		}


		void test_keepConnection()
		{
			{
				const auto& weak = SmartManager::get().toWeakRef();
				const auto& strong = SmartManager::get(true);
				QVERIFY(weak != strong);
			}

			{
				const auto& weak = SmartManager::get(true).toWeakRef();
				const auto& strong = SmartManager::get();
				QVERIFY(weak == strong);
			}

			SmartManager::releaseConnection();
			{
				const auto& weak = SmartManager::get().toWeakRef();
				const auto& strong = SmartManager::get();
				QVERIFY(weak != strong);
			}
		}


		void test_updateInfo_data()
		{
			QTest::addColumn<EidUpdateInfo>("updateInfo");

			QTest::addRow("INTERNAL_ERROR") << EidUpdateInfo::INTERNAL_ERROR;
			QTest::addRow("NO_PROVISIONING") << EidUpdateInfo::NO_PROVISIONING;
			QTest::addRow("UNAVAILABLE") << EidUpdateInfo::UNAVAILABLE;
			QTest::addRow("UPDATE_AVAILABLE") << EidUpdateInfo::UPDATE_AVAILABLE;
			QTest::addRow("UP_TO_DATE") << EidUpdateInfo::UP_TO_DATE;
		}


		void test_updateInfo()
		{
			QFETCH(EidUpdateInfo, updateInfo);

			Env::getSingleton<ReaderManager>()->callExecute([updateInfo] {
					setUpdateInfo(updateInfo);
					QCOMPARE(SmartManager::get()->updateInfo(), updateInfo);
				});
		}


		void test_deleteSmart()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					const auto& smartManager = SmartManager::get();

					QVERIFY(!smartManager->deleteSmart());

					setDeleteSmartEidResult(EidServiceResult::SUCCESS);

					QVERIFY(smartManager->deleteSmart());
					QVERIFY(smartManager->status() == EidStatus::NO_PROVISIONING);
				});
		}


		void test_installSmart()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					const auto& smartManager = SmartManager::get();

					QVERIFY(!smartManager->installSmart());

					setInstallSmartEidResult(EidServiceResult::SUCCESS);

					QVERIFY(smartManager->installSmart());
					QVERIFY(smartManager->status() == EidStatus::NO_PERSONALIZATION);
				});
		}


		void test_deletePersonalization()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					const auto& smartManager = SmartManager::get();

					QVERIFY(!smartManager->deletePersonalization());

					setDeletePersonalizationResult(EidServiceResult::SUCCESS);

					QVERIFY(smartManager->deletePersonalization());
					QVERIFY(smartManager->status() == EidStatus::NO_PERSONALIZATION);
				});
		}


		void test_initializePrePersonalization()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					// The eID interface mock does not support deletion of the Smart-eID personalization, prepare for a brighter future.
					const auto [result, preparePersonalizationData] = SmartManager::get()->initializePersonalization(QString(), QString());
					QVERIFY(result == EidServiceResult::UNDEFINED);
					QVERIFY(preparePersonalizationData == "");
				});
		}


		void test_transmit()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					QByteArray emptyBuffer;
					CommandApdu emptyApdu(emptyBuffer);
					QCOMPARE(SmartManager::get()->transmit(emptyApdu), ResponseApduResult{CardReturnCode::COMMAND_FAILED});
				});
		}


		void test_prepareIdentification()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					QCOMPARE(SmartManager::get()->prepareIdentification(QByteArray()), EstablishPaceChannelOutput(CardReturnCode::COMMAND_FAILED));
				});
		}


		void test_challenge()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					QCOMPARE(SmartManager::get()->challenge(), ResponseApduResult{CardReturnCode::COMMAND_FAILED});
				});
		}


		void test_performTAandCA()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
					const auto emptyBuffer = QByteArray();
					const auto& result = SmartManager::get()->performTAandCA(CVCertificateChain(), emptyBuffer, emptyBuffer, emptyBuffer, emptyBuffer);
					QCOMPARE(result, TerminalAndChipAuthenticationResult{CardReturnCode::COMMAND_FAILED});
				});
		}


};

QTEST_GUILESS_MAIN(test_SmartManager)
#include "test_SmartManager.moc"
