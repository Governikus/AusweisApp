/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetTcToken.h"

#include "ResourceLoader.h"

#include "MockNetworkReply.h"

#include <QByteArrayList>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateGetTcToken
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void test_Run()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			const QUrl validUrl("https://a.not.existing.valid.test.url.com"_L1);
			const QUrl invalidUrl("test"_L1);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			context->setTcTokenUrl(validUrl);
			QTest::ignoreMessage(QtDebugMsg, "Fetch TCToken URL: QUrl(\"https://a.not.existing.valid.test.url.com\")");
			state.run();
			QCOMPARE(spyAbort.count(), 0);

			context->setTcTokenUrl(invalidUrl);
			QTest::ignoreMessage(QtCriticalMsg, "TCToken URL is invalid: QUrl(\"test\")");
			state.run();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Get_TcToken_Invalid_Url);
		}


		void test_IsValidRedirectUrl()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);

			QTest::ignoreMessage(QtCriticalMsg, "Error while connecting to the provider. The server returns an invalid or empty redirect URL.");
			const QUrl emptyUrl;
			QVERIFY(!state.isValidRedirectUrl(emptyUrl));

			const QUrl invalidUrl("test"_L1);
			QVERIFY(!state.isValidRedirectUrl(invalidUrl));

			const QUrl validUrl("https://a.not.existing.valid.test.url.com"_L1);
			QVERIFY(state.isValidRedirectUrl(validUrl));
		}


		void test_SendRequest()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			const QUrl url("https://a.not.existing.valid.test.url.com"_L1);
			state.sendRequest(url);
			QCOMPARE(state.mConnections.size(), 3);

			QTRY_COMPARE(spyAbort.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Get_TcToken_Network_Error);
		}


		void test_ParseTcTokenNoData()
		{
			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			state.mReply.reset(new MockNetworkReply(), &QObject::deleteLater);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Received no data.");
			state.parseTcToken();
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Get_TcToken_Empty_Data);
		}


		void test_ParseTcTokenWithDataUsePsk()
		{
			const QByteArray data("<?xml version=\"1.0\"?>"
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

			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			state.mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy spyContinue(&state, &StateGetTcToken::fireContinue);

			QVERIFY(!context->getTcToken());
			state.parseTcToken();
			QVERIFY(context->getTcToken());
			QVERIFY(!context->isTcTokenNotFound());
			QCOMPARE(spyContinue.count(), 1);
		}


		void test_ParseTcTokenWithDataNoPsk()
		{
			const QByteArray data("invalid data");
			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			state.mReply.reset(new MockNetworkReply(data), &QObject::deleteLater);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "TCToken invalid");
			state.parseTcToken();
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Get_TcToken_Invalid_Data);
		}


};

QTEST_GUILESS_MAIN(test_StateGetTcToken)
#include "test_StateGetTcToken.moc"
