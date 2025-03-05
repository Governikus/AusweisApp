/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginWebService.h"

#include "HttpServerRequestor.h"
#include "MockNetworkManager.h"
#include "ResourceLoader.h"
#include "VersionInfo.h"
#include "WorkflowRequest.h"

#include <QScopedValueRollback>
#include <QtCore>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_UiPluginWebService
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<UiPluginWebService> mUi;
		QScopedPointer<QSignalSpy> mShowUiSpy, mShowUserInfoSpy, mAuthenticationSpy;

		QUrl getUrl(const QString& pUrl)
		{
			const auto& port = QString::number(Env::getShared<HttpServer>()->getServerPort());
			return QUrl(QStringLiteral("http://localhost:%1%2").arg(port, pUrl));
		}


		QString getUserAgentVersion(const QString& pVersion)
		{
			return QStringLiteral("%1/%2 (TR-03124-1/1.4)").arg(VersionInfo::getInstance().getImplementationTitle(), pVersion);
		}


		bool containsHeader(const QSharedPointer<QNetworkReply>& pReply, QByteArrayView pHeader)
		{
			for (const QByteArray& header : pReply->rawHeaderList())
			{
				if (header.toLower() == pHeader)
				{
					return true;
				}
			}
			return false;
		}

	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setApplicationVersion("1.0.0"_L1);
			HttpServer::cPort = 0;
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mUi = QSharedPointer<UiPluginWebService>::create();
			QVERIFY(mUi->initialize());
			mShowUiSpy.reset(new QSignalSpy(mUi.data(), &UiPlugin::fireShowUiRequested));
			mShowUserInfoSpy.reset(new QSignalSpy(mUi.data(), &UiPlugin::fireShowUserInformationRequested));
			mAuthenticationSpy.reset(new QSignalSpy(mUi.data(), &UiPlugin::fireWorkflowRequested));
		}


		void cleanup()
		{
			mUi.clear();
			Env::clear();
		}


		void showUI_data()
		{
			QTest::addColumn<QLatin1String>("url");
			QTest::addColumn<UiModule>("module");

			QTest::newRow("noModule") << QLatin1String("/eID-Client?ShowUI") << UiModule::DEFAULT;
			QTest::newRow("current") << QLatin1String("/eID-Client?ShowUI=current") << UiModule::CURRENT;
			QTest::newRow("PINManagement") << QLatin1String("/eID-Client?ShowUI=PINManagement") << UiModule::PINMANAGEMENT;
			QTest::newRow("PINManagement caseInsensitive") << QLatin1String("/eID-Client?ShowUI=PINManagement") << UiModule::PINMANAGEMENT;
			QTest::newRow("Settings") << QLatin1String("/eID-Client?ShowUI=SETTINGS") << UiModule::SETTINGS;

			QTest::newRow("Tutorial") << QLatin1String("/eID-Client?ShowUI=TUTORIAL") << UiModule::DEFAULT;
			QTest::newRow("Onboarding") << QLatin1String("/eID-Client?ShowUI=ONBOARDING") << UiModule::ONBOARDING;
			QTest::newRow("Help") << QLatin1String("/eID-Client?ShowUI=HELP") << UiModule::HELP;
			QTest::newRow("Self AUth") << QLatin1String("/eID-Client?ShowUI=SELF_AUTHENTICATION") << UiModule::SELF_AUTHENTICATION;

			QTest::newRow("SMART_EID") << QLatin1String("/eID-Client?ShowUI=SMART_EID") << UiModule::SMART_EID;
			QTest::newRow("Smart-eID") << QLatin1String("/eID-Client?ShowUI=Smart-eID") << UiModule::SMART_EID;
		}


		void showUI()
		{
			QFETCH(QLatin1String, url);
			QFETCH(UiModule, module);

			QTest::ignoreMessage(QtDebugMsg, "Request type: showui");

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl(url));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(module));
		}


		void status_data()
		{
			QTest::addColumn<QLatin1String>("url");
			QTest::addColumn<QLatin1String>("header");
			QTest::addColumn<QLatin1String>("name");

			QTest::newRow("default") << QLatin1String("/eID-Client?status") << QLatin1String("text/plain; charset=utf-8") << QLatin1String("PLAIN");
			QTest::newRow("plain") << QLatin1String("/eID-Client?status=plain") << QLatin1String("text/plain; charset=utf-8") << QLatin1String("PLAIN");
			QTest::newRow("json") << QLatin1String("/eID-Client?status=json") << QLatin1String("application/json") << QLatin1String("JSON");
			QTest::newRow("json caseInsensitive") << QLatin1String("/eID-Client?status=JsoN") << QLatin1String("application/json") << QLatin1String("JSON");
		}


		void status()
		{
			QFETCH(QLatin1String, url);
			QFETCH(QLatin1String, header);
			QFETCH(QLatin1String, name);

			QTest::ignoreMessage(QtDebugMsg, "Request type: status");
			QTest::ignoreMessage(QtDebugMsg, QRegularExpression(QStringLiteral("Create response with status format: ") + name));

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl(url));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			const auto& contentType = reply->header(QNetworkRequest::ContentTypeHeader);
			QCOMPARE(contentType.toString(), header);
		}


		void imageRequest_data()
		{
			QTest::addColumn<QLatin1String>("url");
			QTest::addColumn<QLatin1String>("header");
			QTest::addColumn<int>("size");
			QTest::addColumn<QNetworkReply::NetworkError>("error");

			QTest::newRow("favicon") << QLatin1String("/favicon.ico") << QLatin1String("image/x-icon") << 94921 << QNetworkReply::NoError;
			QTest::newRow("smallicon") << QLatin1String("/images/html_templates/icon_attention.svg") << QLatin1String("image/svg+xml") << 957 << QNetworkReply::NoError;
			QTest::newRow("nothing") << QLatin1String("/images/html_templates/nothing.gif") << QLatin1String() << 0 << QNetworkReply::ContentNotFoundError;
		}


		void imageRequest()
		{
			QFETCH(QLatin1String, url);
			QFETCH(QLatin1String, header);
			QFETCH(int, size);
			QFETCH(QNetworkReply::NetworkError, error);

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl(url));
			QVERIFY(reply);
			QCOMPARE(reply->readAll().size(), size);
			QCOMPARE(reply->error(), error);

			const auto& contentType = reply->header(QNetworkRequest::ContentTypeHeader);
			QCOMPARE(contentType.toString(), header);
		}


		void authentication()
		{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 8, 0))
			QSKIP("QTBUG-130666");
#endif
			connect(mUi.data(), &UiPlugin::fireWorkflowRequested, mUi.data(), &UiPlugin::onWorkflowStarted); // fake AppController

			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void authenticationConnectionLost()
		{
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");

			connect(mUi.data(), &UiPlugin::fireWorkflowRequested, this, [this](const QSharedPointer<WorkflowRequest>& pRequest)
					{
						const auto request = pRequest->getData().value<QSharedPointer<HttpRequest>>();
						request->send(HTTP_STATUS_NO_CONTENT); // send something to avoid retry of Qt
						auto* socket = request->take();
						socket->close();
						socket->deleteLater();
						mUi->onWorkflowStarted(pRequest);
					});

			QTest::ignoreMessage(QtCriticalMsg, "Cannot send 'Processing' to caller as connection is lost");
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void authenticationAlreadyActive()
		{
			connect(mUi.data(), &UiPlugin::fireWorkflowRequested, mUi.data(), &UiPlugin::onWorkflowUnhandled);

			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::ContentConflictError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void corsResponseOptions()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.optionsRequest(getUrl("/eID-Client?tctokenURL=bla"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);
			QCOMPARE(reply->readAll().size(), 0);
			QVERIFY(containsHeader(reply, "access-control-allow-origin"));
			QVERIFY(containsHeader(reply, "access-control-allow-private-network"));

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
		}


		void corsResponseHead()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.headRequest(getUrl("/eID-Client?tctokenURL=bla"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);
			QCOMPARE(reply->readAll().size(), 0);
			QVERIFY(containsHeader(reply, "access-control-allow-origin"));
			QVERIFY(containsHeader(reply, "access-control-allow-private-network"));

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
		}


		void unknownRequest()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?unknownRequest"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::ContentNotFoundError);
		}


		void sameUserAgentVersion()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?ShowUI"_L1));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUserInfoSpy->count(), 0);

			const auto& serverHeader = reply->header(QNetworkRequest::ServerHeader);
			QCOMPARE(serverHeader.toString(), getUserAgentVersion(QCoreApplication::applicationVersion()));
		}


		void currentHigherThanCallerUserAgent()
		{
			QTest::ignoreMessage(QtWarningMsg, "Current version is higher than caller version");

			QNetworkRequest request(getUrl("/eID-Client?ShowUI"_L1));
			request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentVersion("0.0.0"_L1));

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(request);
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start an older version"_L1));
		}


		void currentLowerThanCallerUserAgent()
		{
			QTest::ignoreMessage(QtWarningMsg, "Current version is lower than caller version");

			QNetworkRequest request(getUrl("/eID-Client?ShowUI"_L1));
			request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentVersion("2.0.0"_L1));

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(request);
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start a newer version"_L1));
		}


		void proxyDetection_data()
		{
			QTest::addColumn<int>("httpStatusCode");
			QTest::addColumn<int>("existingAppResult");

			QTest::newRow("timeout") << 0 << static_cast<int>(UiPluginWebService::ExistingAppResult::SHOWUI_TIMEOUT);
			QTest::newRow("no-proxy") << 200 << static_cast<int>(UiPluginWebService::ExistingAppResult::SHOWUI_SUCCEED);
			QTest::newRow("proxy-failed") << 502 << static_cast<int>(UiPluginWebService::ExistingAppResult::REBIND_FAILED);
			QTest::newRow("proxy-succedd") << 502 << static_cast<int>(UiPluginWebService::ExistingAppResult::REBIND_SUCCEED);
		}


		void proxyDetection()
		{
			QFETCH(int, httpStatusCode);
			QFETCH(int, existingAppResult);
			const auto existingAppResultType = static_cast<UiPluginWebService::ExistingAppResult>(existingAppResult);

			const auto testAddress = HttpServer::cAddresses.constFirst();
			QScopedValueRollback guard(HttpServer::cAddresses);

			MockNetworkManager networkManager;
			Env::set(NetworkManager::staticMetaObject, &networkManager);

			if (existingAppResultType == UiPluginWebService::ExistingAppResult::SHOWUI_TIMEOUT)
			{
				QTest::ignoreMessage(QtWarningMsg, "ShowUI request timed out");
			}
			else
			{
				auto* reply = new MockNetworkReply();
				reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, httpStatusCode);
				reply->fireFinished();
				networkManager.setNextReply(reply);

				if (existingAppResultType == UiPluginWebService::ExistingAppResult::REBIND_FAILED)
				{
					HttpServer::cAddresses.clear();
				}
			}

			QCOMPARE(mUi->handleExistingApp(HttpServer::cPort, testAddress), existingAppResultType);
		}


		void test_GuessImageContentType_data()
		{
			QTest::addColumn<QLatin1String>("fileName");
			QTest::addColumn<QByteArray>("output");

			QTest::newRow("x-icon") << QLatin1String("test.ico") << QByteArrayLiteral("image/x-icon");
			QTest::newRow("X-ICON") << QLatin1String("test.iCo") << QByteArrayLiteral("image/x-icon");
			QTest::newRow("jpg") << QLatin1String("test.jpg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("JPG") << QLatin1String("test.JPG") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("jpeg") << QLatin1String("test.jpeg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("JPEG") << QLatin1String("test.Jpeg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("png") << QLatin1String("test.png") << QByteArrayLiteral("image/png");
			QTest::newRow("PNG") << QLatin1String("test.pNG") << QByteArrayLiteral("image/png");
			QTest::newRow("svg") << QLatin1String("test.svg") << QByteArrayLiteral("image/svg+xml");
			QTest::newRow("SVG") << QLatin1String("TEST.SVG") << QByteArrayLiteral("image/svg+xml");
			QTest::newRow("other") << QLatin1String("test.test") << QByteArrayLiteral("image");
		}


		void test_GuessImageContentType()
		{
			QFETCH(QLatin1String, fileName);
			QFETCH(QByteArray, output);

			QCOMPARE(mUi->guessImageContentType(fileName), output);
		}


};

QTEST_GUILESS_MAIN(test_UiPluginWebService)
#include "test_UiPluginWebService.moc"
