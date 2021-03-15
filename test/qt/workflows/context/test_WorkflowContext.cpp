/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "context/WorkflowContext.h"

#include "MockCardConnection.h"
#include "MockCardConnectionWorker.h"
#include "MockReader.h"

#include <QtTest>


using namespace governikus;


class test_WorkflowContext
	: public QObject
{
	Q_OBJECT
	QSharedPointer<WorkflowContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new WorkflowContext());
		}


		void cleanup()
		{
			mContext.clear();
		}


		void test_WorkflowFinished()
		{
			mContext->setWorkflowFinished(true);
			QVERIFY(mContext->isWorkflowFinished());

			mContext->setWorkflowFinished(false);
			QVERIFY(!mContext->isWorkflowFinished());
		}


		void test_Can()
		{
			const QString can1 = QStringLiteral("123256");
			const QString can2 = QStringLiteral("222222");
			const QString can3 = QStringLiteral("222222");

			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCanChanged);

			mContext->setCan(can1);
			QCOMPARE(mContext->getCan(), can1);
			QCOMPARE(spy.count(), 1);

			mContext->setCan(can2);
			QCOMPARE(mContext->getCan(), can2);
			QCOMPARE(spy.count(), 2);

			mContext->setCan(can3);
			QCOMPARE(mContext->getCan(), can2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Pin()
		{
			const QString pin1 = QStringLiteral("123256");
			const QString pin2 = QStringLiteral("222222");
			const QString pin3 = QStringLiteral("222222");
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePinChanged);

			mContext->setPin(pin1);
			QCOMPARE(mContext->getPin(), pin1);
			QCOMPARE(spy.count(), 1);

			mContext->setPin(pin2);
			QCOMPARE(mContext->getPin(), pin2);
			QCOMPARE(spy.count(), 2);

			mContext->setPin(pin3);
			QCOMPARE(mContext->getPin(), pin2);
			QCOMPARE(spy.count(), 2);
		}


		void test_Puk()
		{
			const QString puk1 = QStringLiteral("123256789");
			const QString puk2 = QStringLiteral("222222222");
			const QString puk3 = QStringLiteral("222222222");
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePukChanged);

			mContext->setPuk(puk1);
			QCOMPARE(mContext->getPuk(), puk1);
			QCOMPARE(spy.count(), 1);

			mContext->setPuk(puk2);
			QCOMPARE(mContext->getPuk(), puk2);
			QCOMPARE(spy.count(), 2);

			mContext->setPuk(puk3);
			QCOMPARE(mContext->getPuk(), puk2);
			QCOMPARE(spy.count(), 2);
		}


		void test_ErrorReportToUser()
		{
			mContext->setErrorReportedToUser(true);
			QVERIFY(mContext->isErrorReportedToUser());

			mContext->setErrorReportedToUser(false);
			QVERIFY(!mContext->isErrorReportedToUser());
		}


		void test_CurrentState()
		{
			const QString state1 = QStringLiteral("state1");
			const QString state2 = QStringLiteral("state2");
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireStateChanged);

			mContext->setCurrentState(state1);
			QCOMPARE(mContext->getCurrentState(), state1);
			QCOMPARE(spy.count(), 1);
			QVERIFY(!mContext->isStateApproved());

			mContext->setCurrentState(state2);
			QCOMPARE(mContext->getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(!mContext->isStateApproved());

			mContext->killWorkflow();
			QCOMPARE(mContext->getCurrentState(), state2);
			QCOMPARE(spy.count(), 2);
			QVERIFY(mContext->isStateApproved());
		}


		void test_ReaderPlugInTypes()
		{
			QVector<ReaderManagerPlugInType> vector1({ReaderManagerPlugInType::PCSC});
			QVector<ReaderManagerPlugInType> vector2({ReaderManagerPlugInType::REMOTE});
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireReaderPlugInTypesChanged);

			mContext->setReaderPlugInTypes(vector1);
			QCOMPARE(mContext->getReaderPlugInTypes(), vector1);
			QCOMPARE(spy.count(), 1);
			spy.clear();

			mContext->setReaderPlugInTypes(vector2);
			QCOMPARE(mContext->getReaderPlugInTypes(), vector2);
			QCOMPARE(spy.count(), 1);
		}


		void test_LastPaceAndResult()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::firePaceResultUpdated);

			mContext->setLastPaceResult(CardReturnCode::COMMAND_FAILED);
			QCOMPARE(mContext->getLastPaceResult(), CardReturnCode::COMMAND_FAILED);
			QCOMPARE(spy.count(), 1);

			mContext->setLastPaceResult(CardReturnCode::OK);
			QCOMPARE(mContext->getLastPaceResult(), CardReturnCode::OK);
			QCOMPARE(spy.count(), 2);
		}


		void test_CardConnection()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCardConnectionChanged);

			QSharedPointer<CardConnection> cardConnection1(new MockCardConnection());
			mContext->setCardConnection(cardConnection1);
			QCOMPARE(mContext->getCardConnection(), cardConnection1);
			QCOMPARE(spy.count(), 1);

			QSharedPointer<CardConnection> cardConnection2(new MockCardConnection());
			mContext->setCardConnection(cardConnection2);
			QCOMPARE(mContext->getCardConnection(), cardConnection2);
			QCOMPARE(spy.count(), 2);

			mContext->setCardConnection(cardConnection2);
			QCOMPARE(mContext->getCardConnection(), cardConnection2);
			QCOMPARE(spy.count(), 2);
		}


		void test_IsPinBlocked()
		{
			QThread workerThread;
			workerThread.start();

			{
				const QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());
				worker->moveToThread(&workerThread);
				mContext->setCardConnection(QSharedPointer<CardConnection>::create(worker));
				QVERIFY(!mContext->isPinBlocked());

				const CardInfo cardInfo1(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, false, false);
				const ReaderInfo readerInfo1(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo1);
				Q_EMIT worker->fireReaderInfoChanged(readerInfo1);
				QVERIFY(!mContext->isPinBlocked());

				const CardInfo cardInfo2(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 0, false, false);
				const ReaderInfo readerInfo2(QString(), ReaderManagerPlugInType::UNKNOWN, cardInfo2);
				Q_EMIT worker->fireReaderInfoChanged(readerInfo2);
				QVERIFY(mContext->isPinBlocked());
			}

			workerThread.quit();
			workerThread.wait();
		}


		void test_WorkflowKilled()
		{
			QSignalSpy spy(mContext.data(), &WorkflowContext::fireCancelWorkflow);

			QVERIFY(!mContext->isWorkflowKilled());

			QTest::ignoreMessage(QtWarningMsg, "Killing the current workflow.");
			mContext->killWorkflow();
			QVERIFY(mContext->isWorkflowKilled());
			QCOMPARE(mContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Cancellation_By_User);
			QVERIFY(mContext->isStateApproved());
			QCOMPARE(spy.count(), 1);
		}


		void test_IsWorkflowCancelled()
		{
			QVERIFY(!mContext->isWorkflowCancelled());

			Q_EMIT mContext->fireCancelWorkflow();
			QVERIFY(mContext->isWorkflowCancelled());
		}


};

QTEST_GUILESS_MAIN(test_WorkflowContext)
#include "test_WorkflowContext.moc"
