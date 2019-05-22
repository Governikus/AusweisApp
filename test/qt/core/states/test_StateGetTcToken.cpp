/*!
 * \brief Unit tests for \ref StateGetTcToken
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetTcToken.h"

#include "MockNetworkReply.h"

#include <QByteArrayList>
#include <QtTest>


using namespace governikus;


class test_StateGetTcToken
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Run()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);
			const QUrl validUrl(QString("https://test.com"));
			const QUrl invalidUrl(QString("test"));
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			context->setTcTokenUrl(validUrl);
			QTest::ignoreMessage(QtDebugMsg, "Got TC Token URL: QUrl(\"https://test.com\")");
			state.run();
			QCOMPARE(spyAbort.count(), 0);

			context->setTcTokenUrl(invalidUrl);
			QTest::ignoreMessage(QtDebugMsg, "Got TC Token URL: QUrl(\"test\")");
			state.run();
			QCOMPARE(spyAbort.count(), 1);
		}


		void test_IsValidRedirectUrl()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);

			QTest::ignoreMessage(QtCriticalMsg, "Error while connecting to the service provider. The server returns an invalid or empty redirect URL.");
			const QUrl emptyUrl;
			QVERIFY(!state.isValidRedirectUrl(emptyUrl));

			const QUrl invalidUrl(QString("test"));
			QVERIFY(!state.isValidRedirectUrl(invalidUrl));

			const QUrl validUrl(QString("https://test.com"));
			QVERIFY(state.isValidRedirectUrl(validUrl));
		}


		void test_SendRequest()
		{
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);
			const QUrl url(QString("https://test.com"));
			state.sendRequest(url);
			QCOMPARE(state.mConnections.size(), 3);
		}


		void test_ParseTcTokenNoData()
		{
			MockNetworkReply reply;
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);
			state.mReply = QPointer<QNetworkReply>(&reply);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			QTest::ignoreMessage(QtDebugMsg, "Received no data.");
			state.parseTcToken();
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_No_Data_Received);
			QCOMPARE(spyAbort.count(), 1);
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
			MockNetworkReply reply(data);
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);
			state.mReply = QPointer<QNetworkReply>(&reply);
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
			MockNetworkReply reply(data);
			const QSharedPointer<AuthContext> context(new AuthContext(nullptr));
			StateGetTcToken state(context);
			state.mReply = QPointer<QNetworkReply>(&reply);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "TCToken invalid");
			state.parseTcToken();
			QCOMPARE(context->getStatus().getStatusCode(), GlobalStatus::Code::Workflow_TrustedChannel_Server_Format_Error);
			QCOMPARE(spyAbort.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_StateGetTcToken)
#include "test_StateGetTcToken.moc"
