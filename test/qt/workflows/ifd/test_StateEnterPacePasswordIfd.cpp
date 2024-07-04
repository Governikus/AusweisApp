/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEnterPacePasswordIfd.h"

#include "context/IfdServiceContext.h"
#include "states/StateBuilder.h"

#include "MockIfdServer.h"

#include <QtTest>


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


};

QTEST_GUILESS_MAIN(test_StateEnterPacePasswordIfd)
#include "test_StateEnterPacePasswordIfd.moc"
