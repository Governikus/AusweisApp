/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderScanEnabler.h"

#include "ApplicationModel.h"
#include "Env.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"

#include <QSignalSpy>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)


using namespace governikus;


class test_ReaderScanEnabler
	: public QObject
{
	Q_OBJECT

	QScopedPointer<QQuickItem> mParent;
	QPointer<ReaderScanEnabler> mEnabler;

	private Q_SLOTS:
		void init()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			mParent.reset(new QQuickItem());

			auto& mockPlugin = MockReaderManagerPlugin::getInstance();
			mockPlugin.addReader();
			mEnabler = new ReaderScanEnabler(mParent.data());
			mEnabler->setPluginType(mockPlugin.getInfo().getPluginType());
			mEnabler->setVisible(false);
			QCoreApplication::processEvents(); // Make sure to process things from itemChange
		}


		void cleanup()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_enableScan()
		{
			QSignalSpy scanRunningChanged(mEnabler.data(), &ReaderScanEnabler::fireScanRunningChanged);
			const auto& pluginType = mEnabler->getPluginType();
			const auto* readerManager = Env::getSingleton<ReaderManager>();

			QCOMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), false);
			mEnabler->enableScan(true);
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), true);
			QTRY_COMPARE(scanRunningChanged.size(), 1);

			mEnabler->enableScan(true); // Call a second time to ensure the state does not change
			QCOMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), true);
			QTRY_COMPARE(scanRunningChanged.size(), 1);

			mEnabler->enableScan(false);
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), false);
			QTRY_COMPARE(scanRunningChanged.size(), 2);

			mEnabler->enableScan(false); // Call a second time to ensure the state does not change
			QCOMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), false);
			QTRY_COMPARE(scanRunningChanged.size(), 2);
		}


		void test_visibilityChange_data()
		{
			QTest::addColumn<bool>("initialScanState");
			QTest::addColumn<bool>("finalScanState");

			QTest::addRow("Scan not running") << false << false;
			QTest::addRow("Scan already running") << true << true;
		}


		void test_visibilityChange()
		{
			const auto& pluginType = mEnabler->getPluginType();
			auto* readerManager = Env::getSingleton<ReaderManager>();

			QFETCH(bool, initialScanState);
			QFETCH(bool, finalScanState);

			initialScanState ? readerManager->startScan(pluginType)
							 : readerManager->stopScan(pluginType);

			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), initialScanState);
			mEnabler->setVisible(true);
			QVERIFY(mEnabler->isVisible());
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), true);
			mEnabler->setVisible(false);
			QVERIFY(!mEnabler->isVisible());
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), finalScanState);
		}


		void test_restartScan_data()
		{
			QTest::addColumn<QSharedPointer<WorkflowContext>>("workflowContext");
			QTest::addColumn<bool>("finalScanState");

			QTest::addRow("No WorkflowContext") << QSharedPointer<WorkflowContext>() << false;
			QTest::addRow("ChangePinContext") << QSharedPointer<WorkflowContext>(new ChangePinContext()) << true;
		}


		void test_restartScan()
		{
			const auto& pluginType = mEnabler->getPluginType();
			const auto* readerManager = Env::getSingleton<ReaderManager>();

			QFETCH(QSharedPointer<WorkflowContext>, workflowContext);
			QFETCH(bool, finalScanState);

			Env::getSingleton<ApplicationModel>()->resetContext(workflowContext);
			QCOMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), false);
			mEnabler->setVisible(true);
			QVERIFY(mEnabler->isVisible());
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), true);
			mEnabler->setVisible(false);
			QVERIFY(!mEnabler->isVisible());
			QTRY_COMPARE(readerManager->getPluginInfo(pluginType).isScanRunning(), finalScanState);
		}


};


QTEST_MAIN(test_ReaderScanEnabler)
#include "test_ReaderScanEnabler.moc"
