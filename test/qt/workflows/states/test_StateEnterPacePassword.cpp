/**
 * Copyright (c) 2019-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateEnterPacePassword.h"

#include "TestAuthContext.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateEnterPacePassword
	: public QObject
{
	Q_OBJECT
	QSharedPointer<AuthContext> mAuthContext;
	QSharedPointer<StateEnterPacePassword> mState;

	private Q_SLOTS:
		void init()
		{
			mAuthContext.reset(new TestAuthContext(":/paos/DIDAuthenticateEAC1.xml"_L1));
			mState.reset(new StateEnterPacePassword(mAuthContext));
		}


		void cleanup()
		{
			mState.clear();
			mAuthContext.clear();
		}


		void test_Run()
		{
			QSignalSpy spyContinue(mState.data(), &AbstractState::fireContinue);

			mState->run();

			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateEnterPacePassword)
#include "test_StateEnterPacePassword.moc"
