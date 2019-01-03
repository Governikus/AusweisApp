/*!
 * \brief unit tests for \ref StateConnectCard
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateConnectCard.h"

#include "MockCardConnectionWorker.h"

#include <QtTest>

using namespace governikus;

class test_StateConnectCard
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_OnCommandDone()
		{
			QThread workerThread;
			workerThread.start();

			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateConnectCard connectCard(context);
			const QString rName("reader name");
			const QSharedPointer<CreateCardConnectionCommand> command(new CreateCardConnectionCommand(rName, QPointer<ReaderManagerWorker>()));

			QSignalSpy spyContinue(&connectCard, &StateConnectCard::fireContinue);

			QSharedPointer<MockCardConnectionWorker> connectionWorker(new MockCardConnectionWorker());
			connectionWorker->moveToThread(&workerThread);
			QSharedPointer<CardConnection> cardConnection(new CardConnection(connectionWorker));
			command->mCardConnection = cardConnection;
			QTest::ignoreMessage(QtDebugMsg, "Card connection was successful");
			connectCard.onCommandDone(command);
			QCOMPARE(context->getCardConnection(), cardConnection);
			QCOMPARE(spyContinue.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnReaderRemoved()
		{
			const QString readerName = QStringLiteral("name");
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateConnectCard connectCard(context);

			QSignalSpy spy(&connectCard, &StateConnectCard::fireReaderRemoved);

			connectCard.onReaderRemoved(readerName);
			QCOMPARE(spy.count(), 0);

			context->setReaderName(readerName);
			connectCard.onReaderRemoved(readerName);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnAbort()
		{
			const QSharedPointer<WorkflowContext> context(new WorkflowContext());
			StateConnectCard connectCard(context);
			QSignalSpy spyRetry(&connectCard, &StateConnectCard::fireRetry);
			connectCard.onAbort();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateConnectCard)
#include "test_StateConnectCard.moc"
