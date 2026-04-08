/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateConnectCard.h"

#include "Env.h"
#include "ReaderManager.h"

#include "MockCardConnectionWorker.h"
#include "TestHookThread.h"
#include "TestWorkflowContext.h"

#include <QPointer>
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
		QSharedPointer<CreateCardConnectionCommand> createCardConnectionCommand(const QString& readerName, QSharedPointer<CardConnectionWorker> cardConnectionWorker = nullptr)
		{
			const QSharedPointer<CreateCardConnectionCommand> command(new CreateCardConnectionCommand(readerName, QPointer<ReaderManagerWorker>()));

			if (cardConnectionWorker)
			{
				command->mCardConnectionWorker = cardConnectionWorker;
			}

			return command;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
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

			TestHookThread workerThread;

			{
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

				QPointer<MockReader> reader = new MockReader();
				reader->setReaderInfo(ReaderInfo(rName, ReaderManagerPluginType::NFC, CardInfo(CardType::EID_CARD)));
				auto connectionWorker = MockCardConnectionWorker::create(&workerThread, reader);

				mState->onCommandDone(createCardConnectionCommand(rName, connectionWorker));
				QVERIFY(mContext->getCardConnection() != nullptr);
				QCOMPARE(spyContinue.count(), eidTypeMismatch ? 0 : 1);
				QVERIFY(mState->getContext()->getCardInitiallyAppeared());

				mContext->resetCardConnection();
			}
		}


		void test_onUnusableCardConnectionLost()
		{
			TestHookThread workerThread;

			{
				const auto info = ReaderInfo(QStringLiteral("name"));
				QPointer<MockReader> reader = new MockReader();
				reader->setReaderInfo(ReaderInfo(info.getName(), ReaderManagerPluginType::NFC, CardInfo(CardType::EID_CARD)));
				auto connectionWorker = MockCardConnectionWorker::create(&workerThread, reader);
				auto cardConnection = QSharedPointer<CardConnection>(new CardConnection(connectionWorker));

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

				mContext->resetCardConnection();
				cardConnection.reset();
			}
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
