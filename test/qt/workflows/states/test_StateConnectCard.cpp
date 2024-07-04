/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief unit tests for \ref StateConnectCard
 */

#include "states/StateConnectCard.h"

#include "Env.h"
#include "ReaderManager.h"

#include "MockCardConnectionWorker.h"
#include "TestWorkflowContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StateConnectCard
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateConnectCard> mState;
	QSharedPointer<WorkflowContext> mContext;
	ReaderInfo mReaderInfo;

	private:
		QSharedPointer<CardConnection> createCardConnection(QThread& workerThread)
		{
			QSharedPointer<MockCardConnectionWorker> connectionWorker(new MockCardConnectionWorker());
			connectionWorker->moveToThread(&workerThread);
			return QSharedPointer<CardConnection>(new CardConnection(connectionWorker));
		}


		QSharedPointer<CreateCardConnectionCommand> createCardConnectionCommand(const QString& readerName, QSharedPointer<CardConnection> cardConnection = nullptr)
		{
			const QSharedPointer<CreateCardConnectionCommand> command(new CreateCardConnectionCommand(readerName, QPointer<ReaderManagerWorker>()));

			if (cardConnection)
			{
				command->mCardConnection = cardConnection;
			}

			return command;
		}

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
			mReaderInfo = ReaderInfo("test"_L1, ReaderManagerPluginType::UNKNOWN, CardInfo(CardType::EID_CARD));

			mContext.reset(new TestWorkflowContext());
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

			const QString rName("reader name"_L1);

			QSignalSpy spyContinue(mState.data(), &StateConnectCard::fireContinue);
			QSignalSpy spyAbort(mState.data(), &StateConnectCard::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Card connection command completed");
			QTest::ignoreMessage(QtDebugMsg, "Card connection failed");
			mState->onCommandDone(createCardConnectionCommand(rName));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mState->getContext()->getFailureCode(), FailureCode::Reason::Connect_Card_Connection_Failed);

			QTest::ignoreMessage(QtDebugMsg, "Card connection command completed");
			QTest::ignoreMessage(QtDebugMsg, "Card connection was successful");
			const auto& cardConnection = createCardConnection(workerThread);
			mState->onCommandDone(createCardConnectionCommand(rName, cardConnection));
			QCOMPARE(mContext->getCardConnection(), cardConnection);
			QCOMPARE(spyContinue.count(), 1);

			workerThread.quit();
			workerThread.wait();
		}


		void test_onUnusableCardConnectionLost()
		{
			QThread workerThread;
			workerThread.start();

			const auto info = ReaderInfo(QStringLiteral("name"));
			const auto cardConnection = createCardConnection(workerThread);

			QSignalSpy spy(mState.data(), &StateConnectCard::fireRetry);

			mState->onUnusableCardConnectionLost(info);
			QCOMPARE(spy.count(), 0);

			mContext->setReaderName(info.getName());
			mContext->setCardConnection(cardConnection);

			mState->onUnusableCardConnectionLost(ReaderInfo(QStringLiteral("anotherReader")));
			QCOMPARE(spy.count(), 0);
			QCOMPARE(mContext->getReaderName(), info.getName());
			QCOMPARE(mContext->getCardConnection(), cardConnection);

			mState->onUnusableCardConnectionLost(info);
			QCOMPARE(spy.count(), 1);
			QVERIFY(mContext->getReaderName().isEmpty());
			QVERIFY(mContext->getCardConnection().isNull());

			workerThread.quit();
			workerThread.wait();
		}


		void test_OnEntry()
		{
			mState->setObjectName(QStringLiteral("dummyStateName"));

			QSignalSpy spyRetry(mState.data(), &StateConnectCard::fireRetry);

			mState->onEntry(nullptr);

			Q_EMIT mContext->fireReaderPluginTypesChanged();
			QCOMPARE(spyRetry.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateConnectCard)
#include "test_StateConnectCard.moc"
