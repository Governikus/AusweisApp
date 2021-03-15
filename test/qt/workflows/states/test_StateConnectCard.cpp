/*!
 * \brief unit tests for \ref StateConnectCard
 *
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateConnectCard.h"

#include "Env.h"
#include "ReaderManager.h"

#include "MockCardConnectionWorker.h"
#include "MockReader.h"

#include <QtTest>

using namespace governikus;

class test_StateConnectCard
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateConnectCard> mState;
	QSharedPointer<WorkflowContext> mContext;
	ReaderInfo mReaderInfo;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<ReaderManager>()->init();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void init()
		{
			mReaderInfo = ReaderInfo(QString("test"), ReaderManagerPlugInType::UNKNOWN, CardInfo(CardType::EID_CARD));

			mContext.reset(new WorkflowContext());
			mState.reset(new StateConnectCard(mContext));
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_OnCommandDone()
		{
			QThread workerThread;
			workerThread.start();

			const QString rName("reader name");
			const QSharedPointer<CreateCardConnectionCommand> command(new CreateCardConnectionCommand(rName, QPointer<ReaderManagerWorker>()));

			QSignalSpy spyContinue(mState.data(), &StateConnectCard::fireContinue);
			QSignalSpy spyAbort(mState.data(), &StateConnectCard::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Card connection command completed");
			QTest::ignoreMessage(QtDebugMsg, "Card connection failed");
			mState->onCommandDone(command);
			QCOMPARE(spyAbort.count(), 1);

			QSharedPointer<MockCardConnectionWorker> connectionWorker(new MockCardConnectionWorker());
			connectionWorker->moveToThread(&workerThread);
			QSharedPointer<CardConnection> cardConnection(new CardConnection(connectionWorker));
			command->mCardConnection = cardConnection;

			QTest::ignoreMessage(QtDebugMsg, "Card connection command completed");
			QTest::ignoreMessage(QtDebugMsg, "Card connection was successful");
			mState->onCommandDone(command);
			QCOMPARE(mContext->getCardConnection(), cardConnection);
			QCOMPARE(spyContinue.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnReaderRemoved()
		{
			const auto info = ReaderInfo(QStringLiteral("name"));

			QSignalSpy spy(mState.data(), &StateConnectCard::fireRetry);

			mState->onReaderRemoved(info);
			QCOMPARE(spy.count(), 0);

			mContext->setReaderName(info.getName());
			mState->onReaderRemoved(info);
			QCOMPARE(spy.count(), 1);
		}


		void test_OnEntry()
		{
			const QString stateName("name");
			mState->setStateName(stateName);

			QSignalSpy spyRetry(mState.data(), &StateConnectCard::fireRetry);

			mState->onEntry(nullptr);

			Q_EMIT mContext->fireReaderPlugInTypesChanged();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateConnectCard)
#include "test_StateConnectCard.moc"
