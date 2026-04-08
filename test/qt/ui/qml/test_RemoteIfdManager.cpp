/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdManager.h"

#include "ReaderManager.h"
#include "RemoteIfdClient.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_IMPORT_PLUGIN(RemoteIfdReaderManagerPlugin)


class test_RemoteIfdManager
	: public QObject
{
	Q_OBJECT

	private:
		void verifyState(uint pNumber, bool pDetection, bool pScan)
		{
			qDebug() << "Checking number" << pNumber;
			QTRY_COMPARE(Env::getSingleton<RemoteIfdClient>()->isDetecting(), pDetection);
			QTRY_COMPARE(Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::REMOTE_IFD).isScanRunning(), pScan);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(Env::getSingleton<RemoteIfdClient>()->isDetecting(), false);
			QCOMPARE(Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::REMOTE_IFD).isScanRunning(), false);
		}


		void cleanup()
		{
			Env::getSingleton<RemoteIfdClient>()->stopDetection();
			Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPluginType::REMOTE_IFD);
		}


		void stateChangeConditionMissing_data()
		{
			QTest::addColumn<bool>("startEnabled");
			QTest::addColumn<bool>("detectionOnly");

			QTest::newRow("start disabled, detection") << false << true;
			QTest::newRow("start disabled, scan") << false << false;
			QTest::newRow("start enabled, detection") << true << true;
			QTest::newRow("start enabled, scan") << true << false;
		}


		void stateChangeConditionMissing()
		{
			QFETCH(bool, startEnabled);
			QFETCH(bool, detectionOnly);

			if (startEnabled)
			{
				if (detectionOnly)
				{
					Env::getSingleton<RemoteIfdClient>()->startDetection();
				}
				else
				{
					Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPluginType::REMOTE_IFD);
					QTRY_VERIFY(Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::REMOTE_IFD).isScanRunning());
				}
			}

			RemoteIfdManager changer([](){
						return false;
					}, detectionOnly);
			verifyState(1, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(true);
			verifyState(2, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(true);
			verifyState(3, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(false);
			verifyState(4, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(false);
			verifyState(5, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(true);
			verifyState(6, startEnabled, startEnabled && !detectionOnly);
		}


		void stateChangeConditionGiven_data()
		{
			QTest::addColumn<bool>("startEnabled");
			QTest::addColumn<bool>("detectionOnly");

			QTest::newRow("start disabled, detection") << false << true;
			QTest::newRow("start disabled, scan") << false << false;
			QTest::newRow("start enabled, detection") << true << true;
			QTest::newRow("start enabled, scan") << true << false;
		}


		void stateChangeConditionGiven()
		{
			QFETCH(bool, startEnabled);
			QFETCH(bool, detectionOnly);

			if (startEnabled)
			{
				if (detectionOnly)
				{
					Env::getSingleton<RemoteIfdClient>()->startDetection();
				}
				else
				{
					Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPluginType::REMOTE_IFD);
					QTRY_VERIFY(Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::REMOTE_IFD).isScanRunning());
				}
			}

			RemoteIfdManager changer([](){
						return true;
					}, detectionOnly);
			verifyState(1, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(true);
			verifyState(2, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(true);
			verifyState(3, startEnabled, startEnabled && !detectionOnly);

			changer.onApplicationStateChanged(false);
			verifyState(4, false, false);

			changer.onApplicationStateChanged(false);
			verifyState(5, false, false);

			changer.onApplicationStateChanged(true);
			verifyState(6, startEnabled, startEnabled && !detectionOnly);
		}


};

QTEST_GUILESS_MAIN(test_RemoteIfdManager)
#include "test_RemoteIfdManager.moc"
