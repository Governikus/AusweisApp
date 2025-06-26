/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEnterPacePasswordIfd.h"

#include "ReaderManager.h"
#include "context/IfdServiceContext.h"
#include "states/StateBuilder.h"

#include "MockCardConnection.h"
#include "MockIfdServer.h"
#include "MockReaderManagerPlugin.h"

#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateEnterPacePasswordIfd
	: public QObject
{
	Q_OBJECT
	QSharedPointer<MockIfdServer> mIfdServer;
	QSharedPointer<IfdServiceContext> mContext;
	QSharedPointer<StateEnterPacePasswordIfd> mState;

	private Q_SLOTS:
		void init()
		{
			mIfdServer.reset(new MockIfdServer());
			mContext.reset(new IfdServiceContext(mIfdServer));
			mState.reset(StateBuilder::createState<StateEnterPacePasswordIfd>(mContext));
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
			mIfdServer.clear();
		}


		void test_onEntry()
		{
			QSignalSpy spyAbort(mState.data(), &AbstractState::fireAbort);

			mState->onEntry(nullptr);
			QCOMPARE(spyAbort.count(), 0);
			QVERIFY(mContext->getModifyPinMessageResponseApdu().isEmpty());

			Q_EMIT mContext->fireUserError(StatusCode::FILE_NOT_FOUND);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
			spyAbort.clear();

			Q_EMIT mIfdServer->getMessageHandler()->destroyed();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getEstablishPaceChannelOutput().getPaceReturnCode(), CardReturnCode::CANCELLATION_BY_USER);
		}


		void removeCard()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations

			QSignalSpy cardRemoved(readerManager, &ReaderManager::fireCardRemoved);

			const auto& context = mState->getContext();
			MockReaderManagerPlugin::getInstance().addReader("MockReader"_L1);
			auto* reader = MockReaderManagerPlugin::getInstance().getReaders().at(0);
			mState->onEntry(nullptr);

			context->setReaderName("MockWrongReader"_L1);
			context->setCardConnection(QSharedPointer<MockCardConnection>::create());
			Q_EMIT reader->fireCardRemoved(reader->getReaderInfo());
			QTRY_COMPARE(cardRemoved.count(), 1);
			QVERIFY(!context->getCardConnection().isNull());

			context->setReaderName("MockReader"_L1);
			QTest::ignoreMessage(QtDebugMsg, "Card was removed while waiting for user input. Resetting card connection");
			Q_EMIT reader->fireCardRemoved(reader->getReaderInfo());
			QTRY_COMPARE(cardRemoved.count(), 2);
			QVERIFY(context->getCardConnection().isNull());

			readerManager->shutdown();
		}


};

QTEST_GUILESS_MAIN(test_StateEnterPacePasswordIfd)
#include "test_StateEnterPacePasswordIfd.moc"
