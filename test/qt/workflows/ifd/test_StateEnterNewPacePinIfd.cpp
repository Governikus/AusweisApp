/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEnterNewPacePinIfd.h"

#include "context/IfdServiceContext.h"
#include "states/StateBuilder.h"

#include "MockIfdServer.h"

#include <QtTest>


using namespace governikus;


class test_StateEnterNewPacePinIfd
	: public QObject
{
	Q_OBJECT
	QSharedPointer<MockIfdServer> mIfdServer;
	QSharedPointer<IfdServiceContext> mContext;
	QSharedPointer<StateEnterNewPacePinIfd> mState;

	private Q_SLOTS:
		void init()
		{
			mIfdServer.reset(new MockIfdServer());
			mContext.reset(new IfdServiceContext(mIfdServer));
			mState.reset(StateBuilder::createState<StateEnterNewPacePinIfd>(mContext));
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
			QCOMPARE(mContext->getModifyPinMessageResponseApdu().getStatusCode(), StatusCode::FILE_NOT_FOUND);
			spyAbort.clear();

			Q_EMIT mIfdServer->getMessageHandler()->destroyed();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(mContext->getModifyPinMessageResponseApdu().getStatusCode(), StatusCode::INPUT_CANCELLED);
		}


};

QTEST_GUILESS_MAIN(test_StateEnterNewPacePinIfd)
#include "test_StateEnterNewPacePinIfd.moc"
