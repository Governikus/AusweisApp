/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SmartModel.h"

#include "Env.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "mock/eid_applet_interface_mock.h"

#include <QScopeGuard>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(EidStatus)
Q_DECLARE_METATYPE(EidSupportStatus)
Q_DECLARE_METATYPE(EidServiceResult)


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
			info.setCardInfo(CardInfo(CardType::SMART_EID, FileRef(), QSharedPointer<EFCardAccess>(), pRetryCounter, false, false, pPinInitial));
			mSmartReader->setReaderInfo(info);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			MockReader::cMOCKED_READERMANAGER_TYPE = ReaderManagerPluginType::SMART;
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			setSmartEidStatus(EidStatus::PERSONALIZED);
			mSmartReader = MockReaderManagerPlugin::getInstance().addReader("SmartReader"_L1);
			setupSmartReader(false, 3);

			const auto& smartModel = Env::getSingleton<SmartModel>();
			// just to wait until initialization of SmartModel is finished
			QTRY_COMPARE(smartModel->getState(), SmartModel::State::READY);
		}


		void cleanup()
		{
			setupSmartReader(false, 3);
			setSmartEidSupportStatusResult({EidServiceResult::SUCCESS, EidSupportStatus::AVAILABLE});
			setSmartEidStatus(EidStatus::PERSONALIZED);
			Env::getSingleton<SmartModel>()->mStatus = SmartModel::State::READY;
		}


		void cleanupTestCase()
		{
			MockReaderManagerPlugin::getInstance().removeAllReader();
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_getState_data()
		{
			QTest::addColumn<EidStatus>("eidStatus");
			QTest::addColumn<bool>("pinInitial");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<SmartModel::State>("state");

			QTest::newRow("Unusable") << EidStatus::UNUSABLE << false << 3 << SmartModel::State::UNUSABLE;
			QTest::newRow("Internal error") << EidStatus::INTERNAL_ERROR << false << 3 << SmartModel::State::UNAVAILABLE;
			QTest::newRow("No personalization") << EidStatus::NO_PERSONALIZATION << false << 3 << SmartModel::State::NO_PERSONALIZATION;
			QTest::newRow("No provisioning") << EidStatus::NO_PROVISIONING << false << 3 << SmartModel::State::NO_PROVISIONING;
			QTest::newRow("Personalized") << EidStatus::PERSONALIZED << false << 3 << SmartModel::State::READY;
			QTest::newRow("cert_expired") << EidStatus::CERT_EXPIRED << false << 3 << SmartModel::State::UNUSABLE;

			QTest::newRow("Personalized - Retry Counter 0") << EidStatus::PERSONALIZED << false << 0 << SmartModel::State::UNUSABLE;
			QTest::newRow("Personalized - Retry Counter -1") << EidStatus::PERSONALIZED << false << -1 << SmartModel::State::UNUSABLE;
			QTest::newRow("Personalized - Initial PIN") << EidStatus::PERSONALIZED << true << 3 << SmartModel::State::UNUSABLE;

		}


		void test_getState()
		{
			QFETCH(EidStatus, eidStatus);
			QFETCH(SmartModel::State, state);
			QFETCH(bool, pinInitial);
			QFETCH(int, retryCounter);

			const auto& smartModel = Env::getSingleton<SmartModel>();
			if (eidStatus == EidStatus::PERSONALIZED)
			{
				// Allow an independent test execution. Without this the row
				// "Personalized" will fail because no update of QmlSmartState
				// is required and the signal is missing.
				smartModel->mStatus = SmartModel::State::UNUSABLE;
			}
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireStateChanged);

			setSmartEidStatus(eidStatus);
			setupSmartReader(pinInitial, retryCounter);

			smartModel->updateStatus();
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getState(), state);
		}


		void test_deletePersonalization()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();
			QSignalSpy spyDeletePersonalizationDone(smartModel, &SmartModel::fireDeletePersonalizationDone);
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireStateChanged);

			setDeletePersonalizationResult(EidServiceResult::SUCCESS);

			smartModel->deletePersonalization();
			QTRY_COMPARE(spyDeletePersonalizationDone.count(), 1);
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getState(), SmartModel::State::NO_PERSONALIZATION);
		}


		void test_deleteSmart()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();
			QSignalSpy spyDeleteSmartDone(smartModel, &SmartModel::fireDeleteSmartDone);
			QSignalSpy spySmartStateChanged(smartModel, &SmartModel::fireStateChanged);

			setDeleteSmartEidResult(EidServiceResult::SUCCESS);

			smartModel->deleteSmart();
			QTRY_COMPARE(spyDeleteSmartDone.count(), 1);
			QTRY_COMPARE(spySmartStateChanged.count(), 2);
			QCOMPARE(smartModel->getState(), SmartModel::State::NO_PROVISIONING);
		}


		void test_updateStatus()
		{
			const auto& smartModel = Env::getSingleton<SmartModel>();

			setSmartEidStatus(EidStatus::NO_PERSONALIZATION);

			int signalCount = 0;

			const auto connection = connect(smartModel, &SmartModel::fireStateChanged, this, [&signalCount, smartModel](){
						signalCount++;

						switch (signalCount)
						{
							case 1:
								QCOMPARE(smartModel->getState(), SmartModel::State::UPDATING_STATUS);
								break;

							case 2:
								QCOMPARE(smartModel->getState(), SmartModel::State::NO_PERSONALIZATION);
								break;

							default:
								QFAIL("Expected only two signals");
						}
					});
			const auto guard = qScopeGuard([connection] {
						disconnect(connection);
					});

			smartModel->updateStatus();
			QTRY_COMPARE(signalCount, 2);
		}


		void test_onStatusChanged_data()
		{
			QTest::addColumn<ReaderManagerPluginType>("pluginType");
			QTest::addColumn<bool>("expectSignal");

			QTest::addRow("SMART") << ReaderManagerPluginType::SMART << true;
			QTest::addRow("NFC") << ReaderManagerPluginType::NFC << false;
			QTest::addRow("PCSC") << ReaderManagerPluginType::PCSC << false;
			QTest::addRow("UNKNOWN") << ReaderManagerPluginType::UNKNOWN << false;
		}


		void test_onStatusChanged()
		{
			QFETCH(ReaderManagerPluginType, pluginType);
			QFETCH(bool, expectSignal);

			const auto& smartModel = Env::getSingleton<SmartModel>();

			QSignalSpy spy(smartModel, &SmartModel::fireScanRunningChanged);

			smartModel->onStatusChanged(ReaderManagerPluginInfo(pluginType));
			QCOMPARE(spy.count(), expectSignal ? 1 : 0);
		}


		void test_updateSupportInfo_data()
		{
			QTest::addColumn<EidSupportStatus>("eidSupportStatus");
			QTest::addColumn<EidStatus>("eidStatus");
			QTest::addColumn<SmartModel::State>("smartState");

			QTest::newRow("Unavailable1") << EidSupportStatus::UNAVAILABLE << EidStatus::NO_PROVISIONING << SmartModel::State::UNAVAILABLE;
			QTest::newRow("Unavailable2") << EidSupportStatus::UNAVAILABLE << EidStatus::NO_PERSONALIZATION << SmartModel::State::UNAVAILABLE;
			QTest::newRow("Unavailable3") << EidSupportStatus::UNAVAILABLE << EidStatus::PERSONALIZED << SmartModel::State::UNAVAILABLE;
			QTest::newRow("Unavailable4") << EidSupportStatus::UNAVAILABLE << EidStatus::UNUSABLE << SmartModel::State::UNAVAILABLE;
			QTest::newRow("Available1") << EidSupportStatus::AVAILABLE << EidStatus::NO_PROVISIONING << SmartModel::State::NO_PROVISIONING;
			QTest::newRow("Available2") << EidSupportStatus::AVAILABLE << EidStatus::NO_PERSONALIZATION << SmartModel::State::NO_PERSONALIZATION;
			QTest::newRow("Available3") << EidSupportStatus::AVAILABLE << EidStatus::PERSONALIZED << SmartModel::State::READY;
			QTest::newRow("Available4") << EidSupportStatus::AVAILABLE << EidStatus::UNUSABLE << SmartModel::State::UNUSABLE;
			QTest::newRow("UpToDate1") << EidSupportStatus::UP_TO_DATE << EidStatus::NO_PROVISIONING << SmartModel::State::NO_PROVISIONING;
			QTest::newRow("UpToDate2") << EidSupportStatus::UP_TO_DATE << EidStatus::NO_PERSONALIZATION << SmartModel::State::NO_PERSONALIZATION;
			QTest::newRow("UpToDate3") << EidSupportStatus::UP_TO_DATE << EidStatus::PERSONALIZED << SmartModel::State::READY;
			QTest::newRow("UpToDate4") << EidSupportStatus::UP_TO_DATE << EidStatus::UNUSABLE << SmartModel::State::UNUSABLE;
			QTest::newRow("UpdateAvailable1") << EidSupportStatus::UPDATE_AVAILABLE << EidStatus::NO_PROVISIONING << SmartModel::State::NO_PROVISIONING;
			QTest::newRow("UpdateAvailable2") << EidSupportStatus::UPDATE_AVAILABLE << EidStatus::NO_PERSONALIZATION << SmartModel::State::NO_PERSONALIZATION;
			QTest::newRow("UpdateAvailable3") << EidSupportStatus::UPDATE_AVAILABLE << EidStatus::PERSONALIZED << SmartModel::State::READY;
			QTest::newRow("UpdateAvailable4") << EidSupportStatus::UPDATE_AVAILABLE << EidStatus::UNUSABLE << SmartModel::State::UNUSABLE;
			QTest::newRow("InternalError1") << EidSupportStatus::INTERNAL_ERROR << EidStatus::NO_PROVISIONING << SmartModel::State::UNAVAILABLE;
			QTest::newRow("InternalError2") << EidSupportStatus::INTERNAL_ERROR << EidStatus::NO_PERSONALIZATION << SmartModel::State::UNAVAILABLE;
			QTest::newRow("InternalError3") << EidSupportStatus::INTERNAL_ERROR << EidStatus::PERSONALIZED << SmartModel::State::UNAVAILABLE;
			QTest::newRow("InternalError4") << EidSupportStatus::INTERNAL_ERROR << EidStatus::UNUSABLE << SmartModel::State::UNAVAILABLE;
		}


		void test_updateSupportInfo()
		{
			QFETCH(EidSupportStatus, eidSupportStatus);
			QFETCH(EidStatus, eidStatus);
			QFETCH(SmartModel::State, smartState);

			const auto& smartModel = Env::getSingleton<SmartModel>();
			smartModel->mStatus = SmartModel::State::NO_PROVISIONING;

			setSmartEidSupportStatus(eidSupportStatus);
			setSmartEidStatus(eidStatus);
			smartModel->updateSupportInfo();
			QCOMPARE(smartModel->getState(), SmartModel::State::UPDATING_STATUS);
			QTRY_COMPARE(smartModel->getState(), smartState);
		}


		void test_errorStringUpdateSupportInfo_data()
		{
			QTest::addColumn<EidServiceResult>("serviceResult");

			QTest::addRow("Success") << EidServiceResult::SUCCESS;
			QTest::addRow("Unsupported") << EidServiceResult::UNSUPPORTED;
			QTest::addRow("Error") << EidServiceResult::ERROR;
			QTest::addRow("Info") << EidServiceResult::INFO;
			QTest::addRow("NFC not activated") << EidServiceResult::NFC_NOT_ACTIVATED;
			QTest::addRow("Overload protection") << EidServiceResult::OVERLOAD_PROTECTION;
			QTest::addRow("Undefined") << EidServiceResult::UNDEFINED;
			QTest::addRow("Warn") << EidServiceResult::WARN;
			QTest::addRow("Under maintenance") << EidServiceResult::UNDER_MAINTENANCE;
		}


		void test_errorStringUpdateSupportInfo()
		{
			QFETCH(EidServiceResult, serviceResult);

			setSmartEidSupportStatusResult({serviceResult, EidSupportStatus::AVAILABLE});
			auto* smartModel = Env::getSingleton<SmartModel>();
			smartModel->mStatus = SmartModel::State::NO_PROVISIONING;
			smartModel->updateSupportInfo();
			QTRY_VERIFY(smartModel->getState() != SmartModel::State::UPDATING_STATUS);
			QCOMPARE(smartModel->getErrorString().isEmpty(), serviceResult == EidServiceResult::SUCCESS);
		}


		void test_errorStringDeleteSmart_data()
		{
			QTest::addColumn<EidServiceResult>("serviceResult");

			QTest::addRow("Success") << EidServiceResult::SUCCESS;
			QTest::addRow("Unsupported") << EidServiceResult::UNSUPPORTED;
			QTest::addRow("Error") << EidServiceResult::ERROR;
			QTest::addRow("Info") << EidServiceResult::INFO;
			QTest::addRow("NFC not activated") << EidServiceResult::NFC_NOT_ACTIVATED;
			QTest::addRow("Overload protection") << EidServiceResult::OVERLOAD_PROTECTION;
			QTest::addRow("Undefined") << EidServiceResult::UNDEFINED;
			QTest::addRow("Warn") << EidServiceResult::WARN;
			QTest::addRow("Under maintenance") << EidServiceResult::UNDER_MAINTENANCE;
		}


		void test_errorStringDeleteSmart()
		{
			QFETCH(EidServiceResult, serviceResult);

			setDeleteSmartEidResult(serviceResult);
			auto* smartModel = Env::getSingleton<SmartModel>();
			QSignalSpy spyDeleteDone(smartModel, &SmartModel::fireDeleteSmartDone);
			smartModel->deleteSmart();
			QTRY_COMPARE(spyDeleteDone.count(), 1);
			QCOMPARE(smartModel->getErrorString().isEmpty(), serviceResult == EidServiceResult::SUCCESS);
		}


};

QTEST_MAIN(test_SmartModel)
#include "test_SmartModel.moc"
