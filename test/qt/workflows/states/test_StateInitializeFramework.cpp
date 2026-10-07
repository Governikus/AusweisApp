/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#include "states/StateInitializeFramework.h"

#include "paos/retrieve/PaosParser.h"
#include "states/StateBuilder.h"

#include "TestParserHelper.h"

#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
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
			mAuthContext.reset(new AuthContext());
			auto parser = TestParserHelper::create(":/paos/InitializeFramework.xml"_L1);
			auto* pm = PaosParser().parse(parser).release();
			const QSharedPointer<InitializeFramework> initFramework(static_cast<InitializeFramework*>(pm));
			mAuthContext->setInitializeFramework(initFramework);

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
