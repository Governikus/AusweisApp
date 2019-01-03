/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateCheckRefreshAddress.h"

#include "AppSettings.h"
#include "context/AuthContext.h"
#include "Env.h"
#include "MockNetworkManager.h"
#include "states/StateBuilder.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QtCore>
#include <QtTest>
#include <QThread>

using namespace governikus;


class test_StateCheckRefreshAddress
	: public QObject
{
	Q_OBJECT
	QScopedPointer<StateCheckRefreshAddress> mState;
	QScopedPointer<MockNetworkManager> mMockNetworkManager;
	QSharedPointer<AuthContext> mAuthContext;

	Q_SIGNALS:
		void fireStateStart(QEvent* pEvent);

	private Q_SLOTS:
		void init()
		{
			mMockNetworkManager.reset(new MockNetworkManager);
			Env::set(NetworkManager::staticMetaObject, mMockNetworkManager.data());

			mAuthContext.reset(new AuthContext(nullptr));
			mState.reset(StateBuilder::createState<StateCheckRefreshAddress>(mAuthContext));
			connect(this, &test_StateCheckRefreshAddress::fireStateStart, mState.data(), &AbstractState::onEntry, Qt::ConnectionType::DirectConnection);
		}


		void cleanup()
		{
			mState.reset();
			mAuthContext.reset();
			mMockNetworkManager.reset();
		}


		void mappingToCommunicationError()
		{
			const QVector<GlobalStatus::Code> states = QVector<GlobalStatus::Code>()
					<< GlobalStatus::Code::Network_Ssl_Establishment_Error
					<< GlobalStatus::Code::Network_Ssl_Establishment_Error
					<< GlobalStatus::Code::Workflow_Network_Ssl_Connection_Unsupported_Algorithm_Or_Length
					<< GlobalStatus::Code::Workflow_Network_Ssl_Certificate_Unsupported_Algorithm_Or_Length
					<< GlobalStatus::Code::Workflow_Nerwork_Ssl_Hash_Not_In_Certificate_Description
					<< GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url
					<< GlobalStatus::Code::Workflow_Network_Expected_Redirect
					<< GlobalStatus::Code::Workflow_Network_Invalid_Scheme
					<< GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url
					<< GlobalStatus::Code::Network_TimeOut
					<< GlobalStatus::Code::Network_Proxy_Error;

			for (const GlobalStatus::Code state : states)
			{
				const ECardApiResult& result = GlobalStatus(state);
				QCOMPARE(result.getMinor(), ECardApiResult::Minor::AL_Communication_Error);
			}
		}


		void sendGetRequestWithoutAnyAdditionalParameters()
		{
			QUrl redirectUrl("http://localhost:12345/test_StateCheckRefreshAddress/");
			mAuthContext->setStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
			mState->mUrl = redirectUrl;

			mState->sendGetRequest();
			QCOMPARE(mState->mReply->request().url(), redirectUrl);
		}


		void abortIfNotTcToken()
		{
			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mAuthContext->getRefreshUrl().isEmpty());
		}


		void abortIfRefreshAddressIsNotHttps()
		{
			QSharedPointer<TcToken> tcToken(new TcToken("<?xml version=\"1.0\"?>"
														"<TCTokenType>"
														"  <ServerAddress>https://eid-server.example.de/entrypoint</ServerAddress>"
														"  <SessionIdentifier>1A2BB129</SessionIdentifier>"
														"  <RefreshAddress>http://service.example.de/loggedin?7eb39f62</RefreshAddress>"
														"  <Binding> urn:liberty:paos:2006-08 </Binding>"
														"  <PathSecurity-Protocol> urn:ietf:rfc:4279 </PathSecurity-Protocol>"
														"  <PathSecurity-Parameters>"
														"    <PSK> 4BC1A0B5 </PSK>"
														"  </PathSecurity-Parameters>"
														"</TCTokenType>"));
			mAuthContext->setTcToken(tcToken);

			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireContinue);
			Q_EMIT fireStateStart(nullptr);
			mAuthContext->setStateApproved();

			QCOMPARE(spy.count(), 1);
			QVERIFY(mAuthContext->getRefreshUrl().isEmpty());
		}


		void isMatchingSameOriginPolicyInDevMode()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
			QVERIFY(!mState->isMatchingSameOriginPolicyInDevMode());

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(true);
			const QUrl pUrlHttps("https://test/");
			const QUrl pUrlHttp("http://localhost:12345/test_StateCheckRefreshAddress/");
			const QUrl pSubjectUrl("http://localhost:12345/test_StateCheckRefreshAddress/");

			mState->mUrl = pUrlHttps;
			mState->mSubjectUrl = pSubjectUrl;
			QVERIFY(!mState->isMatchingSameOriginPolicyInDevMode());

			mState->mUrl = pUrlHttp;
			QTest::ignoreMessage(QtCriticalMsg, "SOP-Check: Ignoring scheme and port in developer mode.");
			QTest::ignoreMessage(QtCriticalMsg, "  Origin URL: http://localhost:12345/test_StateCheckRefreshAddress/");
			QTest::ignoreMessage(QtCriticalMsg, "  Refresh URL: http://localhost:12345/test_StateCheckRefreshAddress/");
			QVERIFY(mState->isMatchingSameOriginPolicyInDevMode());
		}


		void determinateSubjectUrl()
		{
			const QUrl tcTokenUrl("http://test/");
			mAuthContext->setTcTokenUrl(tcTokenUrl);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
			QTest::ignoreMessage(QtWarningMsg, "No subjectURL/certificate description available, take the TcToken-URL instead");
			QCOMPARE(mState->determineSubjectUrl(), tcTokenUrl);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(true);
			QCOMPARE(mState->determineSubjectUrl(), tcTokenUrl);
		}


		void reportCommunicationError()
		{
			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireAbort);

			QTest::ignoreMessage(QtCriticalMsg, "Card_Communication_Error | \"An error occurred while communicating with the ID card. Please make sure that your ID card is placed correctly on the card reader and try again.\"");
			mState->reportCommunicationError(GlobalStatus::Code::Card_Communication_Error);
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Card_Communication_Error);
			QCOMPARE(spy.count(), 1);

			mAuthContext->setStatus(GlobalStatus::Code::No_Error);

			QTest::ignoreMessage(QtCriticalMsg, "Network_Other_Error | \"An unknown network error occurred.\"");
			mState->reportCommunicationError(GlobalStatus::Code::Network_Other_Error);
			QCOMPARE(mAuthContext->getStatus().getStatusCode(), GlobalStatus::Code::Network_Other_Error);
			QCOMPARE(spy.count(), 2);
		}


		void onNetworkReply_data()
		{
			QTest::addColumn<QNetworkReply::NetworkError>("networkError");
			QTest::addColumn<GlobalStatus::Code>("status");
			QTest::addColumn<int>("statusCode");
			QTest::addColumn<QUrl>("redirectUrl");
			QTest::addColumn<bool>("developerMode");

			QTest::newRow("service unavailable") << QNetworkReply::NetworkError::ServiceUnavailableError << GlobalStatus::Code::Network_ServiceUnavailable << 1 << QUrl("http://test.com/") << false;
			QTest::newRow("timeout") << QNetworkReply::NetworkError::TimeoutError << GlobalStatus::Code::Network_TimeOut << 2 << QUrl() << false;
			QTest::newRow("proxy error") << QNetworkReply::NetworkError::ProxyNotFoundError << GlobalStatus::Code::Network_Proxy_Error << 0 << QUrl("test") << false;
			QTest::newRow("ssl error") << QNetworkReply::NetworkError::SslHandshakeFailedError << GlobalStatus::Code::Network_Ssl_Establishment_Error << 1 << QUrl("https://test.com/") << false;
			QTest::newRow("other error") << QNetworkReply::NetworkError::OperationCanceledError << GlobalStatus::Code::Network_Other_Error << 2 << QUrl("https://test.com/") << false;
			QTest::newRow("no error unexpected status") << QNetworkReply::NetworkError::NoError << GlobalStatus::Code::Workflow_Network_Expected_Redirect << 2 << QUrl("https://test.com/") << false;
			QTest::newRow("no error empty url") << QNetworkReply::NetworkError::NoError << GlobalStatus::Code::Workflow_Network_Empty_Redirect_Url << 302 << QUrl() << false;
			QTest::newRow("no error invalid url") << QNetworkReply::NetworkError::NoError << GlobalStatus::Code::Workflow_Network_Malformed_Redirect_Url << 302 << QUrl("://://") << false;
			QTest::newRow("no error http") << QNetworkReply::NetworkError::NoError << GlobalStatus::Code::Workflow_Network_Invalid_Scheme << 302 << QUrl("http://test.com/") << false;
			QTest::newRow("no error http developer mode") << QNetworkReply::NetworkError::NoError << GlobalStatus::Code::No_Error << 302 << QUrl("http://test.com/") << true;
		}


		void onNetworkReply()
		{
			QFETCH(QNetworkReply::NetworkError, networkError);
			QFETCH(GlobalStatus::Code, status);
			QFETCH(int, statusCode);
			QFETCH(QUrl, redirectUrl);
			QFETCH(bool, developerMode);

			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(developerMode);

			QPointer<MockNetworkReply> reply(new MockNetworkReply());
			mState->mReply = reply;

			const QByteArray headerName("name");
			const QByteArray value("value");
			reply->setRawHeader(headerName, value);
			reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(statusCode));
			reply->setAttribute(QNetworkRequest::RedirectionTargetAttribute, QVariant(redirectUrl));
			reply->setError(networkError, QString());

			mAuthContext->setStatus(GlobalStatus::Code::No_Error);
			QTest::ignoreMessage(QtDebugMsg, R"(Header | name: value)");
			mState->onNetworkReply();
			if (!developerMode)
			{
				QCOMPARE(mAuthContext->getStatus().getStatusCode(), status);
			}
		}


		void fetchServerCertificate_DeveloperMode()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(true);
			const QUrl url("http://test.de/");
			mState->mUrl = url;
			QTest::ignoreMessage(QtWarningMsg, "Refresh URL is http only. Certificate check skipped.");
			mState->fetchServerCertificate();
		}


		void fetchServerCertificate_AlreadyVerified()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
			const QUrl url("https://test.de/");
			mState->mUrl = url;
			mState->mVerifiedRefreshUrlHosts.insert(0, url);
			QTest::ignoreMessage(QtDebugMsg, "SSL certificate already collected for QUrl(\"https://test.de/\")");
			mState->fetchServerCertificate();
		}


		void fetchServerCertificate()
		{
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeveloperMode(false);
			const QUrl url("https://test.de/");
			mState->mUrl = url;
			mState->fetchServerCertificate();
			QCOMPARE(mState->mConnections.size(), 3);
		}


		void doneSuccess()
		{
			const QUrl url("https://test.de/");
			mState->mUrl = url;
			QSignalSpy spy(mState.data(), &StateCheckRefreshAddress::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Determined RefreshUrl: QUrl(\"https://test.de/\")");
			mState->doneSuccess();
			QCOMPARE(mAuthContext->getRefreshUrl(), url);
		}


};

QTEST_GUILESS_MAIN(test_StateCheckRefreshAddress)
#include "test_StateCheckRefreshAddress.moc"
