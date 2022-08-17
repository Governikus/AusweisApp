/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"
#include "states/StateBuilder.h"
#include "states/StateInitializeFramework.h"

#include <QThread>
#include <QtCore>
#include <QtTest>


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
			mState->onEntry(nullptr);
		}


		void run()
		{
			QSignalSpy spy(mState.data(), &StateInitializeFramework::fireContinue);

			mAuthContext->setStateApproved();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanup()
		{
			mAuthContext.reset();
			mState.clear();
		}


};

QTEST_GUILESS_MAIN(test_StateInitializeFramework)
#include "test_StateInitializeFramework.moc"
