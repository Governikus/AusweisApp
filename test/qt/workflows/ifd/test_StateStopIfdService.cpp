/*!
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateStopIfdService.h"

#include "context/IfdServiceContext.h"
#include "states/StateBuilder.h"

#include "MockCardConnectionWorker.h"
#include "MockIfdServer.h"

#include <QtTest>


using namespace governikus;

class test_StateStopIfdService
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateStopIfdService> mState;
	QSharedPointer<IfdServiceContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new IfdServiceContext(QSharedPointer<IfdServer>(new MockIfdServer())));
			mState.reset(StateBuilder::createState<StateStopIfdService>(mContext));
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_Run()
		{
			QSignalSpy spyContinue(mState.data(), &StateStopIfdService::fireContinue);
			mState->onEntry(nullptr);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_OnExit()
		{
			const QString name("name");
			mState->setStateName(name);
			Q_EMIT mContext->fireCancelWorkflow();
			QVERIFY(!mContext->getIfdServer()->isRunning());
		}


};

QTEST_GUILESS_MAIN(test_StateStopIfdService)
#include "test_StateStopIfdService.moc"
