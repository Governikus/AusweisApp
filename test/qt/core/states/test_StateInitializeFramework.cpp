/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "TestFileHelper.h"
#include "controller/AuthController.h"
#include "states/StateBuilder.h"
#include "states/StateInitializeFramework.h"

#include <QThread>
#include <QtCore/QtCore>
#include <QtTest/QtTest>


using namespace governikus;


class test_StateInitializeFramework
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<AuthContext> mAuthContext;
		QSharedPointer<StateInitializeFramework> mState;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void initTestCase()
		{
			mAuthContext.reset(new AuthContext(nullptr));
			auto fileContent = TestFileHelper::readFile(":/paos/InitializeFramework.xml");
			mAuthContext->setInitializeFramework(QSharedPointer<InitializeFramework>(new InitializeFramework(fileContent)));

			mState.reset(StateBuilder::createState<StateInitializeFramework>(mAuthContext));
			connect(this, &test_StateInitializeFramework::fireStateStart, mState.data(), &AbstractState::onEntry);
		}


		void run()
		{
			QSignalSpy spy(mState.data(), &StateInitializeFramework::fireSuccess);

			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
		}


		void cleanup()
		{
			mAuthContext.reset();
			mState.clear();
		}


};

QTEST_GUILESS_MAIN(test_StateInitializeFramework)
#include "test_StateInitializeFramework.moc"
