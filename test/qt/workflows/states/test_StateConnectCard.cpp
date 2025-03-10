/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
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
	QSharedPointer<TestWorkflowContext> mContext;
	ReaderInfo mReaderInfo;

	private:
		QSharedPointer<CardConnection> createCardConnection(QThread& workerThread, const QString& readerName)
		{
			MockReader* reader = new MockReader();
			reader->setReaderInfo(ReaderInfo(readerName, ReaderManagerPluginType::NFC, CardInfo(CardType::EID_CARD)));
			QSharedPointer<MockCardConnectionWorker> connectionWorker(new MockCardConnectionWorker(reader));
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


		void test_OnCommandDone_data()
		{
			QTest::addColumn<bool>("eidTypeMismatch");

			QTest::addRow("No eID type mismatch") << false;
			QTest::addRow("eID type mismatch") << true;
		}


		void test_OnCommandDone()
		{
			QFETCH(bool, eidTypeMismatch);
			if (eidTypeMismatch)
			{
				mContext->setAcceptedEidTypes({AcceptedEidType::SE_ENDORSED});
			}

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
			QVERIFY(!mState->getContext()->getCardInitiallyAppeared());

			QTest::ignoreMessage(QtDebugMsg, "Card connection command completed");
			QTest::ignoreMessage(QtDebugMsg, "Card connection was successful");
			const auto& cardConnection = createCardConnection(workerThread, rName);
			mState->onCommandDone(createCardConnectionCommand(rName, cardConnection));
			QCOMPARE(mContext->getCardConnection(), cardConnection);
			QCOMPARE(spyContinue.count(), eidTypeMismatch ? 0 : 1);
			QVERIFY(mState->getContext()->getCardInitiallyAppeared());

			workerThread.quit();
			workerThread.wait();
		}


		void test_onUnusableCardConnectionLost()
		{
			QThread workerThread;
			workerThread.start();

			const auto info = ReaderInfo(QStringLiteral("name"));
			const auto cardConnection = createCardConnection(workerThread, info.getName());

			QSignalSpy spy(mState.data(), &StateConnectCard::fireRetry);

			mState->onUnusableCardConnectionLost(info);
			QCOMPARE(spy.count(), 0);

			mContext->setReaderName(info.getName());
			mContext->setCardConnection(cardConnection);

			mState->onUnusableCardConnectionLost(ReaderInfo(QStringLiteral("anotherReader")));
			QCOMPARE(spy.count(), 0);
			QCOMPARE(mContext->getReaderName(), info.getName());
			QCOMPARE(mContext->getCardConnection(), cardConnection);
			QVERIFY(!mState->getContext()->getCardInitiallyAppeared());

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
