/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */

#include "states/StatePreparePaceRemote.h"

#include "AppSettings.h"
#include "EstablishPaceChannelParser.h"
#include "states/StateBuilder.h"

#include <QtTest>


using namespace governikus;

class test_StatePreparePaceRemote
	: public QObject
{
	Q_OBJECT
	QSharedPointer<RemoteServiceContext> mContext;
	QSharedPointer<StatePreparePaceRemote> mState;

	static QSharedPointer<const IfdEstablishPaceChannel> createMessage(PacePasswordId pId)
	{
		EstablishPaceChannel builder;
		builder.setPasswordId(pId);
		const auto command = builder.createCommandDataCcid();
		const auto& buffer = command.getBuffer();
		return QSharedPointer<const IfdEstablishPaceChannel>(new IfdEstablishPaceChannel("slot", buffer));
	}

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new RemoteServiceContext());
			mState.reset(StateBuilder::createState<StatePreparePaceRemote>(mContext));
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
			mContext->setEstablishPaceChannelMessage(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceRemote::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceRemote::fireContinue);

			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);

			mContext->setPin(QString("111111"));
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_RunCan()
		{
			const auto& message = createMessage(PacePasswordId::PACE_CAN);
			mContext->setEstablishPaceChannelMessage(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceRemote::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceRemote::fireContinue);

			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);

			mContext->setCan(QString("111111"));
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_RunPuk()
		{
			const auto& message = createMessage(PacePasswordId::PACE_PUK);
			mContext->setEstablishPaceChannelMessage(message);
			QSignalSpy spyEnterPacePassword(mState.data(), &StatePreparePaceRemote::fireEnterPacePassword);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceRemote::fireContinue);

			mContext->setStateApproved();
			QCOMPARE(spyEnterPacePassword.count(), 1);

			mContext->setPuk(QString("111111"));
			mContext->setStateApproved(false);
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_RunUnknown()
		{
			const auto& message = createMessage(PacePasswordId::UNKNOWN);
			mContext->setEstablishPaceChannelMessage(message);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceRemote::fireContinue);

			QTest::ignoreMessage(QtCriticalMsg, "Cannot handle unknown PacePasswordId");
			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_RunPinPadModeFalse()
		{
			Env::getSingleton<AppSettings>()->getRemoteServiceSettings().setPinPadMode(false);
			QSignalSpy spyContinue(mState.data(), &StatePreparePaceRemote::fireContinue);

			mContext->setStateApproved();
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StatePreparePaceRemote)
#include "test_StatePreparePaceRemote.moc"
