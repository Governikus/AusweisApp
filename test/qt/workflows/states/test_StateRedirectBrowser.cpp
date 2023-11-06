/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateRedirectBrowser.h"

#include "TestFileHelper.h"
#include "states/StateBuilder.h"

#include <QThread>
#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_StateRedirectBrowser
	: public QObject
{
	Q_OBJECT

	QSharedPointer<StateRedirectBrowser> getStateRedirectBrowser(AuthContext::BrowserHandler pHandler = AuthContext::BrowserHandler())
	{
		const auto& context = QSharedPointer<AuthContext>::create(true, QUrl(), pHandler);
		QSharedPointer<StateRedirectBrowser> state(StateBuilder::createState<StateRedirectBrowser>(context));
		state->onEntry(nullptr);
		return state;
	}

	private Q_SLOTS:
		void noError()
		{
			auto state = getStateRedirectBrowser();
			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			state->run();
			QCOMPARE(state->getContext()->getStatus(), GlobalStatus::Code::No_Error);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyContinue.count(), 1);
		}


		void error()
		{
			auto state = getStateRedirectBrowser([](const QSharedPointer<AuthContext>&){
					return QStringLiteral("failing");
				});

			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			QTest::ignoreMessage(QtCriticalMsg, "Cannot send page to caller: \"failing\"");
			state->run();
			QCOMPARE(state->getContext()->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_Browser_Transmission_Error);
			QCOMPARE(state->getContext()->getStatus().getExternalInfo(), QLatin1String("failing"));
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(spyContinue.count(), 0);
		}


};

QTEST_GUILESS_MAIN(test_StateRedirectBrowser)
#include "test_StateRedirectBrowser.moc"
