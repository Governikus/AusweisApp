/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "WorkflowModel.h"

#include "Env.h"
#include "MockCardConnectionWorker.h"
#include "MockReaderManagerPlugin.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QFile>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_WorkflowModel
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

			ResourceLoader::getInstance().init();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void test_ResetContext()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QSignalSpy spyWorkflowStarted(&model, &WorkflowModel::fireWorkflowStarted);
			QSignalSpy spyCurrentStateChanged(&model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyStateEntered(&model, &WorkflowModel::fireStateEntered);
			QSignalSpy spyResultChanged(&model, &WorkflowModel::fireResultChanged);
			QSignalSpy spyReaderPluginTypeChanged(&model, &WorkflowModel::fireReaderPluginTypeChanged);
			QSignalSpy spySelectedReaderChanged(&model, &WorkflowModel::fireSelectedReaderChanged);
			QSignalSpy spyWorkflowFinished(&model, &WorkflowModel::fireWorkflowFinished);

			model.resetWorkflowContext();
			QCOMPARE(spyWorkflowStarted.count(), 0);
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyResultChanged.count(), 1);
			QCOMPARE(spyWorkflowFinished.count(), 1);

			model.resetWorkflowContext(context);
			QCOMPARE(spyWorkflowStarted.count(), 1);
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyStateEntered.count(), 0);
			QCOMPARE(spyResultChanged.count(), 2);
			QCOMPARE(spyWorkflowFinished.count(), 1);

			Q_EMIT context->fireStateChanged("state"_L1);
			QCOMPARE(spyCurrentStateChanged.count(), 3);
			QCOMPARE(spyStateEntered.count(), 1);

			Q_EMIT context->fireResultChanged();
			QCOMPARE(spyResultChanged.count(), 3);

			Q_EMIT context->fireReaderPluginTypesChanged();
			QCOMPARE(spyReaderPluginTypeChanged.count(), 1);

			Q_EMIT context->fireCardConnectionChanged();
			QCOMPARE(spySelectedReaderChanged.count(), 1);
		}


		void test_IsBasicReader()
		{
			QThread connectionThread;
			connectionThread.start();

			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QVERIFY(model.isBasicReader());

			model.mContext = context;
			QVERIFY(model.isBasicReader());

			MockReader reader;
			reader.setInfoBasicReader(false);
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker(&reader));
			worker->moveToThread(&connectionThread);
			QSharedPointer<CardConnection> connection(new CardConnection(worker));
			context->setCardConnection(connection);
			model.mContext = context;
			QVERIFY(!model.isBasicReader());

			connectionThread.quit();
			connectionThread.wait();
		}


		void test_ReaderPluginType()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.mContext = context;
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.setReaderPluginType(ReaderManagerPluginType::UNKNOWN);
			QVERIFY(context->getReaderPluginTypes().contains(ReaderManagerPluginType::UNKNOWN));
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::UNKNOWN);

			model.setReaderPluginType(ReaderManagerPluginType::NFC);
			QVERIFY(context->getReaderPluginTypes().contains(ReaderManagerPluginType::NFC));
			QCOMPARE(model.getReaderPluginType(), ReaderManagerPluginType::NFC);
		}


		void test_isCurrentSmartCardAllowed()
		{
			WorkflowModel workflowModel;
			QVERIFY2(!workflowModel.isCurrentSmartCardAllowed(), "Ensure that this will not crash when the model has no WorkflowContext.");
		}


		void test_startScanExplicitly()
		{
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireReaderPluginTypesChanged);

			WorkflowModel model;
			model.resetWorkflowContext(context);

			model.startScanExplicitly();
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toBool(), true);
		}


		void test_hasCard()
		{
			WorkflowModel model;
			QCOMPARE(model.hasCard(), false);

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.hasCard(), false);

			auto mockReader = MockReaderManagerPlugin::getInstance().addReader("SomeReaderWithCard"_L1);
			auto info = mockReader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD, FileRef(), QSharedPointer<EFCardAccess>(), 3, false, false, false));
			mockReader->setReaderInfo(info);
			model.setReaderPluginType(ReaderManagerPluginType::MOCK);
			QCOMPARE(model.hasCard(), true);
		}


		void test_isPukInoperative()
		{
			WorkflowModel model;
			QVERIFY(!model.isPukInoperative());

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QVERIFY(!model.isPukInoperative());

			context->setStatus(GlobalStatus::Code::Card_Invalid_Pin);
			QVERIFY(!model.isPukInoperative());

			context->setStatus(GlobalStatus::Code::Card_Puk_Blocked);
			QVERIFY(model.isPukInoperative());
		}


		void test_getLastReturnCode()
		{
			WorkflowModel model;
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::UNDEFINED);

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::OK);

			context->setLastPaceResult(CardReturnCode::CANCELLATION_BY_USER);
			QCOMPARE(model.getLastReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
		}


};

QTEST_MAIN(test_WorkflowModel)
#include "test_WorkflowModel.moc"
