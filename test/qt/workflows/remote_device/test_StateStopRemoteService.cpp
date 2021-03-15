/*!
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateStopRemoteService.h"

#include "context/RemoteServiceContext.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "MockRemoteServer.h"

#include <QtTest>


using namespace governikus;

class test_StateStopRemoteService
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateStopRemoteService> mState;
	QSharedPointer<RemoteServiceContext> mContext;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::setCreator<RemoteServer*>(std::function<RemoteServer* ()>([&] {
						return new MockRemoteServer();
					}));
		}


		void init()
		{
			mContext.reset(new RemoteServiceContext());
			mState.reset(StateBuilder::createState<StateStopRemoteService>(mContext));
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_Run()
		{
			QSignalSpy spyContinue(mState.data(), &StateStopRemoteService::fireContinue);
			mState->onEntry(nullptr);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_OnExit()
		{
			const QString name("name");
			mState->setStateName(name);
			Q_EMIT mContext->fireCancelWorkflow();
			QVERIFY(!mContext->getRemoteServer()->isRunning());
		}


};

QTEST_GUILESS_MAIN(test_StateStopRemoteService)
#include "test_StateStopRemoteService.moc"
