/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
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
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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
			QTest::addColumn<EidSupportStatus>("supportInfo");

			QTest::addRow("INTERNAL_ERROR") << EidSupportStatus::INTERNAL_ERROR;
			QTest::addRow("AVAILABLE") << EidSupportStatus::AVAILABLE;
			QTest::addRow("UNAVAILABLE") << EidSupportStatus::UNAVAILABLE;
			QTest::addRow("UPDATE_AVAILABLE") << EidSupportStatus::UPDATE_AVAILABLE;
			QTest::addRow("UP_TO_DATE") << EidSupportStatus::UP_TO_DATE;
		}


		void test_updateInfo()
		{
			QFETCH(EidSupportStatus, supportInfo);

			Env::getSingleton<ReaderManager>()->callExecute([supportInfo] {
						setSmartEidSupportStatus(supportInfo);
						QCOMPARE(SmartManager::get()->updateSupportInfo().mStatus, supportInfo);
					});
		}


		void test_deleteSmart()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
						const auto& smartManager = SmartManager::get();

						QCOMPARE(smartManager->deleteSmart(), EidServiceResult::UNSUPPORTED);

						setDeleteSmartEidResult(EidServiceResult::SUCCESS);

						QCOMPARE(smartManager->deleteSmart(), EidServiceResult::SUCCESS);
						QVERIFY(smartManager->status() == EidStatus::NO_PROVISIONING);
					});
		}


		void test_installSmart()
		{
			Env::getSingleton<ReaderManager>()->callExecute([] {
						const auto& smartManager = SmartManager::get();

						QCOMPARE(smartManager->installSmart(), EidServiceResult::UNSUPPORTED);

						setInstallSmartEidResult(EidServiceResult::SUCCESS);

						QCOMPARE(smartManager->installSmart(), EidServiceResult::SUCCESS);
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
