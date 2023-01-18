/*!
 * \brief Unit tests for \ref SmartModel
 *
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SmartModel.h"

#include "Env.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "SmartManager.h"
#include "mock/eid_applet_interface_mock.h"

#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;


class test_SmartModel
	: public QObject
{
	Q_OBJECT

	private:
		QPointer<MockReader> mSmartReader;

		void setupSmartReader(bool pPinInitial, int pRetryCounter)
		{
			Q_ASSERT(mSmartReader);

			auto info = mSmartReader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::SMART_EID, QSharedPointer<EFCardAccess>(), pRetryCounter, false, false, pPinInitial));
			mSmartReader->setReaderInfo(info);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			MockReader::cMOCKED_READERMANAGER_TYPE = ReaderManagerPlugInType::SMART;
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished

			setSmartEidStatus(EidStatus::PERSONALIZED);
			mSmartReader = MockReaderManagerPlugIn::getInstance().addReader("SmartReader");
			setupSmartReader(false, 3);

			const auto& smartModel = Env::getSingleton<SmartModel>();
			// just to wait until initialization of SmartModel is finished
			QTRY_COMPARE(smartModel->getSmartState(), SmartModel::QmlSmartState::SMART_READY);
		}


		void cleanupTestCase()
		{
			MockReaderManagerPlugIn::getInstance().removeAllReader();
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_getSmartState_data()
		{
			QTest::addColumn<EidStatus>("eidStatus");
			QTest::addColumn<bool>("pinInitial");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<SmartModel::QmlSmartState>("smartState");

			QTest::newRow("Unusable") << EidStatus::APPLET_UNUSABLE << false << 3 << SmartModel::QmlSmartState::SMART_UNUSABLE;
			QTest::newRow("Internal error") << EidStatus::INTERNAL_ERROR << false << 3 << SmartModel::QmlSmartState::SMART_UNAVAILABLE;
			QTest::newRow("No personalization") << EidStatus::NO_PERSONALIZATION << false << 3 << SmartModel::QmlSmartState::SMART_NO_PERSONALIZATION;
			QTest::newRow("No provisioning") << EidStatus::NO_PROVISIONING << false << 3 << SmartModel::QmlSmartState::SMART_NO_PROVISIONING;
			QTest::newRow("Personalized") << EidStatus::PERSONALIZED << false << 3 << SmartModel::QmlSmartState::SMART_READY;
			QTest::newRow("Unavailable") << EidStatus::UNAVAILABLE << false << 3 << SmartModel::QmlSmartState::SMART_UNAVAILABLE;

			QTest::newRow("Personalized - Retry Counter 0") << EidStatus::PERSONALIZED << false << 0 << SmartModel::QmlSmartState::SMART_UNUSABLE;
			QTest::newRow("Personalized - Retry Counter -1") << EidStatus::PERSONALIZED << false << -1 << SmartModel::QmlSmartState::SMART_UNUSABLE;
			QTest::newRow("Personalized - Initial PIN") << EidStatus::PERSONALIZED << true << 3 << SmartModel::QmlSmartState::SMART_UNUSABLE;

		}


		void test_getSmartState()
		{
			QFETCH(EidStatus, eidStatus);
			QFETCH(SmartModel::QmlSmartState, smartState);
			QFETCH(bool, pinInitial);
			QFETCH(int, retryCounter);

			const auto& smartModel = Env::getSingleton<SmartModel>();
			if (eidStatus == EidStatus::PERSONALIZED)
			{
				// Allow an independent test execution. Without this the row
				// "Personalized" will fail because no update of QmlSmartState
				// is required and the signal is missing.
				smartModel->mStatus = SmartModel::QmlSmartState::SMART_UNUSABLE;
			}
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireSmartStateChanged);

			setSmartEidStatus(eidStatus);
			setupSmartReader(pinInitial, retryCounter);

			smartModel->updateStatus();
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getSmartState(), smartState);
		}


		void test_deletePersonalization()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();
			QSignalSpy spyDeletePersonalizationDone(smartModel, &SmartModel::fireDeletePersonalizationDone);
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireSmartStateChanged);

			setDeletePersonalizationResult(EidServiceResult::SUCCESS);

			smartModel->deletePersonalization();
			QTRY_COMPARE(spyDeletePersonalizationDone.count(), 1);
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getSmartState(), SmartModel::QmlSmartState::SMART_NO_PERSONALIZATION);
		}


		void test_deleteSmart()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();
			QSignalSpy spyDeleteSmartDone(smartModel, &SmartModel::fireDeleteSmartDone);
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireSmartStateChanged);

			setDeleteSmartEidResult(EidServiceResult::SUCCESS);

			smartModel->deleteSmart();
			QTRY_COMPARE(spyDeleteSmartDone.count(), 1);
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getSmartState(), SmartModel::QmlSmartState::SMART_NO_PROVISIONING);
		}


		void test_updateStatus()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();

			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);

			int signalCount = 0;

			connect(smartModel, &SmartModel::fireSmartStateChanged, [&signalCount, smartModel](){
					signalCount++;

					switch (signalCount)
					{
							case 1:
								QCOMPARE(smartModel->getSmartState(), SmartModel::QmlSmartState::SMART_UPDATING_STATUS);
								break;

							case 2:
								QCOMPARE(smartModel->getSmartState(), SmartModel::QmlSmartState::SMART_NO_PERSONALIZATION);
								break;

							default:
								QFAIL("Expected only two signals");
					}
				});

			smartModel->updateStatus();
			QTRY_COMPARE(signalCount, 2);
		}


		void test_onStatusChanged_data()
		{
			QTest::addColumn<ReaderManagerPlugInType>("plugInType");
			QTest::addColumn<bool>("expectSignal");

			QTest::addRow("SMART") << ReaderManagerPlugInType::SMART << true;
			QTest::addRow("NFC") << ReaderManagerPlugInType::NFC << false;
			QTest::addRow("PCSC") << ReaderManagerPlugInType::PCSC << false;
			QTest::addRow("UNKNOWN") << ReaderManagerPlugInType::UNKNOWN << false;
		}


		void test_onStatusChanged()
		{
			QFETCH(ReaderManagerPlugInType, plugInType);
			QFETCH(bool, expectSignal);

			const auto& smartModel = Env::getSingleton<SmartModel>();

			QSignalSpy spy(smartModel, &SmartModel::fireScanRunningChanged);

			smartModel->onStatusChanged(ReaderManagerPlugInInfo(plugInType));
			QCOMPARE(spy.count(), expectSignal ? 1 : 0);
		}


};

QTEST_GUILESS_MAIN(test_SmartModel)
#include "test_SmartModel.moc"
