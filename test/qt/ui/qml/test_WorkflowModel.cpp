/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref WorkflowModel
 */

#include "WorkflowModel.h"

#include "Env.h"
#include "MockCardConnectionWorker.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "TestWorkflowContext.h"

#include <QDebug>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;


class test_WorkflowModel
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_ResetContext()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QSignalSpy spyCurrentStateChanged(&model, &WorkflowModel::fireCurrentStateChanged);
			QSignalSpy spyResultChanged(&model, &WorkflowModel::fireResultChanged);
			QSignalSpy spyReaderPlugInTypeChanged(&model, &WorkflowModel::fireReaderPlugInTypeChanged);
			QSignalSpy spySelectedReaderChanged(&model, &WorkflowModel::fireSelectedReaderChanged);

			model.resetWorkflowContext();
			QCOMPARE(spyCurrentStateChanged.count(), 1);
			QCOMPARE(spyResultChanged.count(), 1);

			model.resetWorkflowContext(context);
			QCOMPARE(spyCurrentStateChanged.count(), 2);
			QCOMPARE(spyResultChanged.count(), 2);

			Q_EMIT context->fireStateChanged(QString("state"));
			QCOMPARE(spyCurrentStateChanged.count(), 3);

			Q_EMIT context->fireResultChanged();
			QCOMPARE(spyResultChanged.count(), 3);

			Q_EMIT context->fireReaderPlugInTypesChanged();
			QCOMPARE(spyReaderPlugInTypeChanged.count(), 1);

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


		void test_ReaderPlugInType()
		{
			WorkflowModel model;
			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());

			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.mContext = context;
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.setReaderPlugInType(ReaderManagerPlugInType::UNKNOWN);
			QVERIFY(context->getReaderPlugInTypes().contains(ReaderManagerPlugInType::UNKNOWN));
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::UNKNOWN);

			model.setReaderPlugInType(ReaderManagerPlugInType::NFC);
			QVERIFY(context->getReaderPlugInTypes().contains(ReaderManagerPlugInType::NFC));
			QCOMPARE(model.getReaderPlugInType(), ReaderManagerPlugInType::NFC);
		}


		void test_isSmartCardAllowed()
		{
			QVERIFY(true);
		}


		void test_startScanExplicitly()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			QSignalSpy spy(context.data(), &WorkflowContext::fireReaderPlugInTypesChanged);

			WorkflowModel model;
			model.resetWorkflowContext(context);

			model.startScanExplicitly();
			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toBool(), true);
		}


		void test_hasCard()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished

			WorkflowModel model;
			QCOMPARE(model.hasCard(), false);

			QSharedPointer<WorkflowContext> context(new TestWorkflowContext());
			model.resetWorkflowContext(context);
			QCOMPARE(model.hasCard(), false);

			auto mockReader = MockReaderManagerPlugIn::getInstance().addReader("SomeReaderWithCard");
			auto info = mockReader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::EID_CARD, QSharedPointer<EFCardAccess>(), 3, false, false, false));
			mockReader->setReaderInfo(info);
			model.setReaderPlugInType(ReaderManagerPlugInType::MOCK);
			QCOMPARE(model.hasCard(), true);
		}


};

QTEST_MAIN(test_WorkflowModel)
#include "test_WorkflowModel.moc"
