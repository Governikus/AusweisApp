/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateRedirectBrowser.h"

#include "states/StateBuilder.h"

#include <QThread>
#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
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
		void majorMinor()
		{
			const QString URL_PREFIX("https://www.der-pott-kocht.net:8443/index.html"_L1);
			const QUrl url(URL_PREFIX);

			// Ok
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Ok, ECardApiResult::Minor::null)).toString(),
					URL_PREFIX + "?ResultMajor=ok"_L1);

			// General server error
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Unknown_Error, QString(), ECardApiResult::Origin::Server)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=serverError"_L1);

			// Minors defined in TR-03112-1 and TR-03124-1 2.7.4.2
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Communication_Error)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=communicationError"_L1);
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=trustedChannelEstablishmentFailed"_L1);
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::SAL_Cancellation_by_User)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=cancellationByUser"_L1);

			// No difference between client and server origin
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::SAL_Cancellation_by_User, QString(), ECardApiResult::Origin::Server)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=cancellationByUser"_L1);

			// General client error
			QCOMPARE(StateRedirectBrowser::addMajorMinor(url, ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Not_Initialized)).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=clientError"_L1);
		}


		void validUrl_data()
		{
			QTest::addColumn<GlobalStatus>("status");
			QTest::addColumn<ECardApiResult>("result");
			QTest::addColumn<QUrl>("url");

			QTest::newRow("statusOK_resultOK")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Ok, ECardApiResult::Minor::null)
				<< QUrl("https://www.foo.bar?ResultMajor=ok"_L1);
			QTest::newRow("statusOK_resultERROR_1-5")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_No_Permission, QString(), ECardApiResult::Origin::Server)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=serverError"_L1);
			QTest::newRow("statusOK_resultERROR_2-5")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_No_Permission)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=clientError"_L1);
			QTest::newRow("statusOK_resultERROR_3-5")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=trustedChannelEstablishmentFailed"_L1);
			QTest::newRow("statusOK_resultERROR_4-5")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Communication_Error)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=communicationError"_L1);
			QTest::newRow("statusOK_resultERROR_5-5")
				<< GlobalStatus(GlobalStatus::Code::No_Error)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::SAL_Cancellation_by_User)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=cancellationByUser"_L1);
			QTest::newRow("statusERROR_resultOK")
				<< GlobalStatus(GlobalStatus::Code::Workflow_Cancellation_By_User)
				<< ECardApiResult(ECardApiResult::Major::Ok, ECardApiResult::Minor::null)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=cancellationByUser"_L1);
			QTest::newRow("statusERROR_resultERROR")
				<< GlobalStatus(GlobalStatus::Code::Workflow_Cancellation_By_User)
				<< ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_No_Permission)
				<< QUrl("https://www.foo.bar?ResultMajor=error&ResultMinor=cancellationByUser"_L1);
		}


		void validUrl()
		{
			QFETCH(GlobalStatus, status);
			QFETCH(ECardApiResult, result);
			QFETCH(QUrl, url);

			auto state = getStateRedirectBrowser();
			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			auto context = state->getContext();
			context->setRefreshUrl(QUrl("https://www.foo.bar"_L1));
			context->setStatus(status);
			context->setStartPaosResult(result);
			state->run();
			QCOMPARE(context->getRefreshUrl(), url);
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyContinue.count(), 1);
		}


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


		void emptyRefreshUrlNoCommunicationErrorAddress()
		{
			auto state = getStateRedirectBrowser();
			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			const auto& context = state->getContext();
			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK> 4BC1A0B5 </PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");
			context->setTcToken(QSharedPointer<TcToken>::create(tokenData));
			QTest::ignoreMessage(QtDebugMsg, "Refresh URL is not valid: QUrl(\"\")");
			QTest::ignoreMessage(QtDebugMsg, "CommunicationErrorAddress is not valid: QUrl(\"\")");

			state->run();
			QCOMPARE(context->getRefreshUrl(), QString());
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyContinue.count(), 1);
		}


		void emptyRefreshUrl()
		{
			auto state = getStateRedirectBrowser();
			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			const auto& context = state->getContext();
			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
									   "  <SessionIdentifier>1A2BB129</SessionIdentifier>"
									   "  <RefreshAddress>https://service.example.de/loggedin?7eb39f62</RefreshAddress>"
									   "  <CommunicationErrorAddress>https://flupp</CommunicationErrorAddress>"
									   "  <Binding> urn:liberty:paos:2006-08 </Binding>"
									   "  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK> 4BC1A0B5 </PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");

			context->setTcTokenNotFound(false);
			context->setTcToken(QSharedPointer<TcToken>::create(tokenData));
			QTest::ignoreMessage(QtDebugMsg, "Refresh URL is not valid: QUrl(\"\")");

			state->run();
			QCOMPARE(context->getRefreshUrl(), QStringLiteral("https://flupp?ResultMajor=error&ResultMinor=communicationError"));
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyContinue.count(), 1);
		}


		void emptyExceptCommunicationErrorAddress()
		{
			auto state = getStateRedirectBrowser();
			QSignalSpy spyAbort(state.data(), &StateRedirectBrowser::fireAbort);
			QSignalSpy spyContinue(state.data(), &StateRedirectBrowser::fireContinue);

			const auto& context = state->getContext();
			const QByteArray tokenData("<?xml version=\"1.0\"?>"
									   "<TCTokenType>"
									   "  <ServerAddress></ServerAddress>"
									   "  <SessionIdentifier></SessionIdentifier>"
									   "  <RefreshAddress></RefreshAddress>"
									   "  <CommunicationErrorAddress>https://flupp</CommunicationErrorAddress>"
									   "  <Binding></Binding>"
									   "  <PathSecurity-Protocol></PathSecurity-Protocol>"
									   "  <PathSecurity-Parameters>"
									   "    <PSK></PSK>"
									   "  </PathSecurity-Parameters>"
									   "</TCTokenType>");

			context->setTcTokenNotFound(false);
			QTest::ignoreMessage(QtCriticalMsg, "PathSecurity-Protocol is no valid URI: \"\"");
			context->setTcToken(QSharedPointer<TcToken>::create(tokenData));
			QTest::ignoreMessage(QtDebugMsg, "Refresh URL is not valid: QUrl(\"\")");

			state->run();
			QCOMPARE(context->getRefreshUrl(), QStringLiteral("https://flupp?ResultMajor=error&ResultMinor=communicationError"));
			QCOMPARE(spyAbort.count(), 0);
			QCOMPARE(spyContinue.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateRedirectBrowser)
#include "test_StateRedirectBrowser.moc"
