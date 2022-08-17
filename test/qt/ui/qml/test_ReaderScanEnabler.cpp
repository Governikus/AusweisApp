/*!
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderScanEnabler.h"

#include "ApplicationModel.h"
#include "Env.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "context/ChangePinContext.h"

#include <QSignalSpy>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)


using namespace governikus;


class test_ReaderScanEnabler
	: public QObject
{
	Q_OBJECT

	QSharedPointer<ReaderScanEnabler> mEnabler;

	private Q_SLOTS:
		void init()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished

			auto& mockPlugin = MockReaderManagerPlugIn::getInstance();
			mockPlugin.addReader();
			mEnabler.reset(new ReaderScanEnabler());
			mEnabler->setPlugInType(mockPlugin.getInfo().getPlugInType());
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
			const auto& pluginType = mEnabler->getPlugInType();
			const auto* readerManager = Env::getSingleton<ReaderManager>();

			QCOMPARE(readerManager->isScanRunning(pluginType), false);
			mEnabler->enableScan(true);
			QCOMPARE(readerManager->isScanRunning(pluginType), true);
			QTRY_COMPARE(scanRunningChanged.size(), 1);

			mEnabler->enableScan(true); // Call a second time to ensure the state does not change
			QCOMPARE(readerManager->isScanRunning(pluginType), true);
			QTRY_COMPARE(scanRunningChanged.size(), 1);

			mEnabler->enableScan(false);
			QCOMPARE(readerManager->isScanRunning(pluginType), false);
			QTRY_COMPARE(scanRunningChanged.size(), 2);

			mEnabler->enableScan(false); // Call a second time to ensure the state does not change
			QCOMPARE(readerManager->isScanRunning(pluginType), false);
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
			const auto& pluginType = mEnabler->getPlugInType();
			auto* readerManager = Env::getSingleton<ReaderManager>();

			QFETCH(bool, initialScanState);
			QFETCH(bool, finalScanState);

			initialScanState ? readerManager->startScan(pluginType)
							 : readerManager->stopScan(pluginType);

			QCOMPARE(readerManager->isScanRunning(pluginType), initialScanState);
			mEnabler->setVisible(true);
			QTRY_COMPARE(readerManager->isScanRunning(pluginType), true);
			mEnabler->setVisible(false);
			QTRY_COMPARE(readerManager->isScanRunning(pluginType), finalScanState);
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
			const auto& pluginType = mEnabler->getPlugInType();
			const auto* readerManager = Env::getSingleton<ReaderManager>();

			QFETCH(QSharedPointer<WorkflowContext>, workflowContext);
			QFETCH(bool, finalScanState);

			Env::getSingleton<ApplicationModel>()->resetContext(workflowContext);
			QCOMPARE(readerManager->isScanRunning(pluginType), false);
			mEnabler->setVisible(true);
			QTRY_COMPARE(readerManager->isScanRunning(pluginType), true);
			mEnabler->setVisible(false);
			QTRY_COMPARE(readerManager->isScanRunning(pluginType), finalScanState);
		}


};


QTEST_GUILESS_MAIN(test_ReaderScanEnabler)
#include "test_ReaderScanEnabler.moc"
