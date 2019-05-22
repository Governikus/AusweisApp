/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "context/WorkflowContext.h"

#include "MockCardConnectionWorker.h"
#include "MockReader.h"

#include <QtTest>


using namespace governikus;


class test_WorkflowContext
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_WorkflowFinished()
		{
			WorkflowContext workflowContext;

			workflowContext.setWorkflowFinished(true);
			QVERIFY(workflowContext.isWorkflowFinished());

			workflowContext.setWorkflowFinished(false);
			QVERIFY(!workflowContext.isWorkflowFinished());
		}


		void test_CanAllowed()
		{
			WorkflowContext workflowContext;
			QSignalSpy spy(&workflowContext, &WorkflowContext::fireCanAllowedModeChanged);

			workflowContext.setCanAllowedMode(true);
			QVERIFY(workflowContext.isCanAllowedMode());
			QCOMPARE(spy.count(), 1);

			workflowContext.setCanAllowedMode(false);
			QVERIFY(!workflowContext.isCanAllowedMode());
			QCOMPARE(spy.count(), 2);
		}


		void test_Can()
		{
			WorkflowContext workflowContext;
			const QString can1 = QStringLiteral("123256");
			const QString can2 = QStringLiteral("222222");
			const QString can3 = QStringLiteral("222222");

			QSignalSpy spy(&workflowContext, &WorkflowContext::fireCanChanged);

			workflowContext.setCan(can1);
			QCOMPARE(workflowContext.getCan(), can1);
			QCOMPARE(spy.count(), 1);

			workflowContext.setCan(can2);
			QCOMPARE(workflowContext.getCan(), can2);
			QCOMPARE(spy.count(), 2);

			workflowContext.setCan(can3);
			QCOMPARE(workflowContext.getCan(), can2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Pin()
		{
			WorkflowContext workflowContext;
			const QString pin1 = QStringLiteral("123256");
			const QString pin2 = QStringLiteral("222222");
			const QString pin3 = QStringLiteral("222222");
			QSignalSpy spy(&workflowContext, &WorkflowContext::firePinChanged);

			workflowContext.setPin(pin1);
			QCOMPARE(workflowContext.getPin(), pin1);
			QCOMPARE(spy.count(), 1);

			workflowContext.setPin(pin2);
			QCOMPARE(workflowContext.getPin(), pin2);
			QCOMPARE(spy.count(), 2);

			workflowContext.setPin(pin3);
			QCOMPARE(workflowContext.getPin(), pin2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Puk()
		{
			WorkflowContext workflowContext;
			const QString puk1 = QStringLiteral("123256789");
			const QString puk2 = QStringLiteral("222222222");
			const QString puk3 = QStringLiteral("222222222");
			QSignalSpy spy(&workflowContext, &WorkflowContext::firePukChanged);

			workflowContext.setPuk(puk1);
			QCOMPARE(workflowContext.getPuk(), puk1);
			QCOMPARE(spy.count(), 1);

			workflowContext.setPuk(puk2);
			QCOMPARE(workflowContext.getPuk(), puk2);
			QCOMPARE(spy.count(), 2);

			workflowContext.setPuk(puk3);
			QCOMPARE(workflowContext.getPuk(), puk2);
			QCOMPARE(spy.count(), 2);
		}


		void test_ErrorReportToUser()
		{
			WorkflowContext workflowContext;

			workflowContext.setErrorReportedToUser(true);
			QVERIFY(workflowContext.isErrorReportedToUser());

			workflowContext.setErrorReportedToUser(false);
			QVERIFY(!workflowContext.isErrorReportedToUser());
		}


		void test_CurrentState()
		{
			WorkflowContext workflowContext;
			const QString state1 = QStringLiteral("state1");
			const QString state2 = QStringLiteral("state2");
			QSignalSpy spy(&workflowContext, &WorkflowContext::fireStateChanged);

			workflowContext.setCurrentState(state1);
			QCOMPARE(workflowContext.getCurrentState(), state1);
			QCOMPARE(spy.count(), 1);
			QVERIFY(!workflowContext.isStateApproved());

			workflowContext.setCurrentState(state2);
			QCOMPARE(workflowContext.getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(!workflowContext.isStateApproved());

			workflowContext.killWorkflow();
			QCOMPARE(workflowContext.getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(workflowContext.isStateApproved());
		}


		void test_ReaderPlugInTypes()
		{
			WorkflowContext workflowContext;
			QVector<ReaderManagerPlugInType> vector1({ReaderManagerPlugInType::PCSC});
			QVector<ReaderManagerPlugInType> vector2({ReaderManagerPlugInType::BLUETOOTH});
			QSignalSpy spy(&workflowContext, &WorkflowContext::fireReaderPlugInTypesChanged);

			workflowContext.setReaderPlugInTypes(vector1);
			QCOMPARE(workflowContext.getReaderPlugInTypes(), vector1);
			QCOMPARE(spy.count(), 1);
			spy.clear();

			workflowContext.setReaderPlugInTypes(vector2);
			QCOMPARE(workflowContext.getReaderPlugInTypes(), vector2);
			QCOMPARE(spy.count(), 1);
		}


		void test_LastPaceAndResult()
		{
			WorkflowContext workflowContext;
			QSignalSpy spy(&workflowContext, &WorkflowContext::fireLastPaceResultChanged);

			workflowContext.setLastPaceResult(CardReturnCode::COMMAND_FAILED);
			QCOMPARE(workflowContext.getLastPaceResult(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(spy.count(), 1);

			workflowContext.setLastPaceResult(CardReturnCode::OK);
			QCOMPARE(workflowContext.getLastPaceResult(), CardReturnCode::OK);
			QCOMPARE(spy.count(), 2);
		}


		void test_CardConnection()
		{
			QThread cardThread;
			cardThread.start();
			WorkflowContext workflowContext;
			QSignalSpy spy(&workflowContext, &WorkflowContext::fireCardConnectionChanged);

			MockReader reader1(QStringLiteral("reader"));
			reader1.moveToThread(&cardThread);
			QSharedPointer<CardConnectionWorker> worker1 = CardConnectionWorker::create(&reader1);
			worker1->moveToThread(&cardThread);
			QSharedPointer<CardConnection> cardConnection1(new CardConnection(worker1));

			MockReader reader2(QStringLiteral("reader2"));
			reader2.moveToThread(&cardThread);
			QSharedPointer<CardConnectionWorker> worker2 = CardConnectionWorker::create(&reader2);
			worker2->moveToThread(&cardThread);
			QSharedPointer<CardConnection> cardConnection2(new CardConnection(worker2));

			workflowContext.setCardConnection(cardConnection1);
			QCOMPARE(workflowContext.getCardConnection(), cardConnection1);
			QCOMPARE(spy.count(), 1);

			workflowContext.setCardConnection(cardConnection2);
			QCOMPARE(workflowContext.getCardConnection(), cardConnection2);
			QCOMPARE(spy.count(), 2);

			cardThread.quit();
			cardThread.wait();
		}


		void test_IsPinBlocked()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
			worker->moveToThread(&workerThread);
			const QSharedPointer<CardConnection> connection1(new CardConnection(worker));
			const QSharedPointer<CardConnection> connection2(new CardConnection(worker));
			const CardInfo cardInfo1(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, false, false);
			const CardInfo cardInfo2(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, false, false);
			const ReaderInfo readerInfo1(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo1);
			const ReaderInfo readerInfo2(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo2);

			connection1->onReaderInfoChanged(readerInfo1);
			connection2->onReaderInfoChanged(readerInfo2);

			WorkflowContext context;
			QVERIFY(!context.isPinBlocked());

			context.setCardConnection(connection1);
			QVERIFY(!context.isPinBlocked());

			context.setCardConnection(connection2);
			QVERIFY(context.isPinBlocked());

			workerThread.quit();
			workerThread.wait();
		}


		void test_WorkflowKilled()
		{
			WorkflowContext context;
			QSignalSpy spy(&context, &WorkflowContext::fireCancelWorkflow);

			QVERIFY(!context.isWorkflowKilled());

			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			context.killWorkflow();
			QVERIFY(context.isWorkflowKilled());
			QCOMPARE(context.getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QVERIFY(context.isStateApproved());
			QCOMPARE(spy.count(), 1);
		}


		void test_IsWorkflowCancelled()
		{
			WorkflowContext context;

			QVERIFY(!context.isWorkflowCancelled());

			context.mWorkflowCancelled = true;
			QVERIFY(context.isWorkflowCancelled());
		}


};

QTEST_GUILESS_MAIN(test_WorkflowContext)
#include "test_WorkflowContext.moc"
