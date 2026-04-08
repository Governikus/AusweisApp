/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateGetTcToken.h"

#include "HttpServer.h"
#include "ResourceLoader.h"

#include "MockNetworkManager.h"
#include "MockNetworkReply.h"

#include <QByteArrayList>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_StateGetTcToken
	: public QObject
{
	Q_OBJECT

	private:
		MockNetworkManager mMockNetworkManager;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			HttpServer::cPort = 0;
		}


		void init()
		{
			Env::set(NetworkManager::staticMetaObject, &mMockNetworkManager);
		}


		void cleanup()
		{
			Env::clear();
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

			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), HTTP_STATUS_OK);
			reply->setError(QNetworkReply::UnknownNetworkError, QString());
			mMockNetworkManager.setNextReply(reply);

			state.sendRequest(QUrl("https://a.not.existing.valid.test.url.com"_L1));
			QCOMPARE(state.mConnections.size(), 3);

			QTest::ignoreMessage(QtCriticalMsg, "An error occurred: QNetworkReply::UnknownNetworkError \"Unknown error\"");
			mMockNetworkManager.fireFinished();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode(), FailureCode::Reason::Get_TcToken_Network_Error);
		}


		void test_HttpError_FailureCode_data()
		{
			QTest::addColumn<QNetworkReply::NetworkError>("errorCode");
			QTest::addColumn<llhttp_status>("httpStatusCode");
			QTest::addColumn<FailureCode>("failureCode");

			QTest::newRow("see other") << QNetworkReply::NoError << HTTP_STATUS_SEE_OTHER << FailureCode(FailureCode::Reason::Get_TcToken_Invalid_Redirect_Url);
			QTest::newRow("found") << QNetworkReply::NoError << HTTP_STATUS_FOUND << FailureCode(FailureCode::Reason::Get_TcToken_Invalid_Redirect_Url);
			QTest::newRow("temp redirect") << QNetworkReply::NoError << HTTP_STATUS_TEMPORARY_REDIRECT << FailureCode(FailureCode::Reason::Get_TcToken_Invalid_Redirect_Url);

			QTest::newRow("perm redirect") << QNetworkReply::NoError << HTTP_STATUS_PERMANENT_REDIRECT << FailureCode(FailureCode::Reason::Get_TcToken_Invalid_Server_Reply);
			QTest::newRow("no content") << QNetworkReply::NoError << HTTP_STATUS_NO_CONTENT << FailureCode(FailureCode::Reason::Get_TcToken_Invalid_Server_Reply);

			QTest::newRow("bad request") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_BAD_REQUEST << FailureCode(FailureCode::Reason::Get_TcToken_Client_Error);
			QTest::newRow("not found") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_NOT_FOUND << FailureCode(FailureCode::Reason::Get_TcToken_Client_Error);

			QTest::newRow("service unavailable") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_SERVICE_UNAVAILABLE << FailureCode(FailureCode::Reason::Get_TcToken_ServiceUnavailable);
			QTest::newRow("gateway timeout") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_GATEWAY_TIMEOUT << FailureCode(FailureCode::Reason::Get_TcToken_Server_Error);
			QTest::newRow("bad gateway") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_BAD_GATEWAY << FailureCode(FailureCode::Reason::Get_TcToken_Server_Error);
			QTest::newRow("server error") << QNetworkReply::UnknownNetworkError << HTTP_STATUS_INTERNAL_SERVER_ERROR << FailureCode(FailureCode::Reason::Get_TcToken_Server_Error);
		}


		void test_HttpError_FailureCode()
		{
			QFETCH(QNetworkReply::NetworkError, errorCode);
			QFETCH(llhttp_status, httpStatusCode);
			QFETCH(FailureCode, failureCode);

			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			MockNetworkReply* const reply = new MockNetworkReply(QByteArray(), httpStatusCode);
			reply->setError(errorCode, QString());
			mMockNetworkManager.setNextReply(reply);

			state.sendRequest(QUrl("http://localhost/dummy"_L1));
			QCOMPARE(state.mConnections.size(), 3);

			mMockNetworkManager.fireFinished();
			QCOMPARE(spyAbort.count(), 1);
			QCOMPARE(context->getFailureCode().value(), failureCode);
		}


		void test_Redirect_data()
		{
			QTest::addColumn<QLatin1String>("target");
			QTest::addColumn<QLatin1String>("redirect");

			QTest::newRow("relative") << "index.html"_L1 << "https://a.not.existing.valid.test.url.com/index.html"_L1;
			QTest::newRow("absolute") << "https://another.not.existing.valid.test.url.com/index.html"_L1 << "https://another.not.existing.valid.test.url.com/index.html"_L1;
		}


		void test_Redirect()
		{
			QFETCH(QLatin1String, target);
			QFETCH(QLatin1String, redirect);

			const QSharedPointer<AuthContext> context(new AuthContext());
			StateGetTcToken state(context);
			QSignalSpy spyAbort(&state, &StateGetTcToken::fireAbort);

			MockNetworkReply* const redirectReply = new MockNetworkReply(QByteArray(), HTTP_STATUS_OK);
			redirectReply->setError(QNetworkReply::UnknownNetworkError, QString());
			mMockNetworkManager.setNextReply(redirectReply);

			auto reply = QSharedPointer<MockNetworkReply>::create();
			reply->setRequest(QNetworkRequest(QUrl("https://a.not.existing.valid.test.url.com/tcToken"_L1)));
			reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(302));
			reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute, QVariant(target));
			state.mReply = reply;

			QTest::ignoreMessage(QtDebugMsg, "Status Code: 302 \"FOUND\"");
			state.onNetworkReply();
			QCOMPARE(state.mReply->request().url(), QUrl(redirect));

			QTest::ignoreMessage(QtCriticalMsg, "An error occurred: QNetworkReply::UnknownNetworkError \"Unknown error\"");
			mMockNetworkManager.fireFinished();
			QCOMPARE(spyAbort.count(), 1);
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
