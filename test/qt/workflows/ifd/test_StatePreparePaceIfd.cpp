/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePaceIfd.h"

#include "AppSettings.h"
#include "RemoteIfdServer.h"
#include "states/StateBuilder.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_StatePreparePaceIfd
	: public QObject
{
	Q_OBJECT
	QSharedPointer<IfdServiceContext> mContext;
	QSharedPointer<StatePreparePaceIfd> mState;

	static QSharedPointer<const IfdEstablishPaceChannel> createMessage(PacePasswordId pId)
	{
		EstablishPaceChannel establishPaceChannel(pId);
		return QSharedPointer<const IfdEstablishPaceChannel>(new IfdEstablishPaceChannel("slot"_L1, establishPaceChannel, 6));
	}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new IfdServiceContext(QSharedPointer<IfdServer>(new RemoteIfdServer())));
			mState.reset(StateBuilder::createState<StatePreparePaceIfd>(mContext));
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(true);

			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mContext.clear();
			mState.clear();
		}


		void test_RunPin()
		{
			const auto& message = createMessage(PacePasswordId::PACE_PIN);
			mContext->setEstablishPaceChannel(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceIfd::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceIfd::fireContinue);

			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.count(), 0);

			mContext->setPin("111111"_L1);
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_RunCan()
		{
			const auto& message = createMessage(PacePasswordId::PACE_CAN);
			mContext->setEstablishPaceChannel(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceIfd::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceIfd::fireContinue);

			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.count(), 0);

			mContext->setCan("111111"_L1);
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_RunPuk()
		{
			const auto& message = createMessage(PacePasswordId::PACE_PUK);
			mContext->setEstablishPaceChannel(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceIfd::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceIfd::fireContinue);

			mContext->setStateApproved();
			QTRY_COMPARE(spyEnterPacePassword.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyContinue.count(), 0);

			mContext->setPuk("111111"_L1);
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_RunUnknown()
		{
			const auto& message = createMessage(PacePasswordId::UNKNOWN);
			mContext->setEstablishPaceChannel(message);
			QSignalSpy spyAbort(mState.data(), &StatePreparePaceIfd::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "Cannot handle unknown PacePasswordId");
			mContext->setStateApproved();
			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(mContext->getFailureCode(), FailureCode::Reason::Prepare_Pace_Ifd_Unknown);
		}


		void test_RunPinPadModeFalse()
		{
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceIfd::fireContinue);

			mContext->setStateApproved();
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePaceIfd)
#include "test_StatePreparePaceIfd.moc"
