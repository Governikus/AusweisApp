/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref WebserviceActivationHandler
 */

#include "UIPlugInWebService.h"

#include "HttpServerRequestor.h"
#include "MockNetworkManager.h"
#include "ResourceLoader.h"
#include "VersionInfo.h"
#include "WorkflowRequest.h"

#include <QScopedValueRollback>
#include <QtCore>
#include <QtTest>


using namespace governikus;


class test_UIPlugInWebService
	: public QObject
{
	Q_OBJECT
	QSharedPointer<UIPlugInWebService> mUi;
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

	private Q_SLOTS:
		void initTestCase()
		{
			QCoreApplication::setApplicationVersion("1.0.0");
			HttpServer::cPort = 0;
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mUi = QSharedPointer<UIPlugInWebService>::create();
			QVERIFY(mUi->initialize());
			mShowUiSpy.reset(new QSignalSpy(mUi.data(), &UIPlugIn::fireShowUiRequested));
			mShowUserInfoSpy.reset(new QSignalSpy(mUi.data(), &UIPlugIn::fireShowUserInformationRequested));
			mAuthenticationSpy.reset(new QSignalSpy(mUi.data(), &UIPlugIn::fireWorkflowRequested));
		}


		void cleanup()
		{
			mUi.clear();
			Env::clear();
		}


		void showUI_data()
		{
			QTest::addColumn<QString>("url");
			QTest::addColumn<UiModule>("module");

			QTest::newRow("noModule") << QString("/eID-Client?ShowUI") << UiModule::DEFAULT;
			QTest::newRow("current") << QString("/eID-Client?ShowUI=current") << UiModule::CURRENT;
			QTest::newRow("PINManagement") << QString("/eID-Client?ShowUI=PINManagement") << UiModule::PINMANAGEMENT;
			QTest::newRow("PINManagement caseInsensitive") << QString("/eID-Client?ShowUI=PINManagement") << UiModule::PINMANAGEMENT;
			QTest::newRow("Settings") << QString("/eID-Client?ShowUI=SETTINGS") << UiModule::SETTINGS;

			QTest::newRow("Tutorial") << QString("/eID-Client?ShowUI=TUTORIAL") << UiModule::TUTORIAL;
			QTest::newRow("Help") << QString("/eID-Client?ShowUI=HELP") << UiModule::HELP;
			QTest::newRow("Self AUth") << QString("/eID-Client?ShowUI=SELF_AUTHENTICATION") << UiModule::SELF_AUTHENTICATION;

			QTest::newRow("SMART_EID") << QString("/eID-Client?ShowUI=SMART_EID") << UiModule::SMART_EID;
			QTest::newRow("Smart-eID") << QString("/eID-Client?ShowUI=Smart-eID") << UiModule::SMART_EID;
		}


		void showUI()
		{
			QFETCH(QString, url);
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
			QTest::addColumn<QString>("url");
			QTest::addColumn<QString>("header");
			QTest::addColumn<QString>("name");

			QTest::newRow("default") << QString("/eID-Client?status") << QString("text/plain; charset=utf-8") << QString("PLAIN");
			QTest::newRow("plain") << QString("/eID-Client?status=plain") << QString("text/plain; charset=utf-8") << QString("PLAIN");
			QTest::newRow("json") << QString("/eID-Client?status=json") << QString("application/json") << QString("JSON");
			QTest::newRow("json caseInsensitive") << QString("/eID-Client?status=JsoN") << QString("application/json") << QString("JSON");
		}


		void status()
		{
			QFETCH(QString, url);
			QFETCH(QString, header);
			QFETCH(QString, name);

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
			QTest::addColumn<QString>("url");
			QTest::addColumn<QString>("header");
			QTest::addColumn<int>("size");
			QTest::addColumn<QNetworkReply::NetworkError>("error");

			QTest::newRow("favicon") << QString("/favicon.ico") << QString("image/x-icon") << 94921 << QNetworkReply::NoError;
			QTest::newRow("smallicon") << QString("/images/html_templates/icon_attention.svg") << QString("image/svg+xml") << 957 << QNetworkReply::NoError;
			QTest::newRow("nothing") << QString("/images/html_templates/nothing.gif") << QString() << 0 << QNetworkReply::ContentNotFoundError;
		}


		void imageRequest()
		{
			QFETCH(QString, url);
			QFETCH(QString, header);
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
			connect(mUi.data(), &UIPlugIn::fireWorkflowRequested, mUi.data(), &UIPlugIn::onWorkflowStarted); // fake AppController

			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void authenticationConnectionLost()
		{
			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");

			connect(mUi.data(), &UIPlugIn::fireWorkflowRequested, this, [this](const QSharedPointer<WorkflowRequest>& pRequest)
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
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void authenticationAlreadyActive()
		{
			connect(mUi.data(), &UIPlugIn::fireWorkflowRequested, mUi.data(), &UIPlugIn::onWorkflowUnhandled);

			QTest::ignoreMessage(QtDebugMsg, "Request type: authentication");
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?tctokenURL=bla"));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::ContentConflictError);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void unknownRequest()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?unknownRequest"));
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::ContentNotFoundError);
		}


		void sameUserAgentVersion()
		{
			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(getUrl("/eID-Client?ShowUI"));
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

			QNetworkRequest request(getUrl("/eID-Client?ShowUI"));
			request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentVersion("0.0.0"));

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(request);
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start an older version"));
		}


		void currentLowerThanCallerUserAgent()
		{
			QTest::ignoreMessage(QtWarningMsg, "Current version is lower than caller version");

			QNetworkRequest request(getUrl("/eID-Client?ShowUI"));
			request.setHeader(QNetworkRequest::UserAgentHeader, getUserAgentVersion("2.0.0"));

			HttpServerRequestor requestor;
			QSharedPointer<QNetworkReply> reply = requestor.getRequest(request);
			QVERIFY(reply);
			QCOMPARE(reply->error(), QNetworkReply::NoError);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start a newer version"));
		}


		void proxyDetection_data()
		{
			QTest::addColumn<int>("httpStatusCode");
			QTest::addColumn<int>("existingAppResult");

			QTest::newRow("timeout") << 0 << static_cast<int>(UIPlugInWebService::ExistingAppResult::SHOWUI_TIMEOUT);
			QTest::newRow("no-proxy") << 200 << static_cast<int>(UIPlugInWebService::ExistingAppResult::SHOWUI_SUCCEED);
			QTest::newRow("proxy-failed") << 502 << static_cast<int>(UIPlugInWebService::ExistingAppResult::REBIND_FAILED);
			QTest::newRow("proxy-succedd") << 502 << static_cast<int>(UIPlugInWebService::ExistingAppResult::REBIND_SUCCEED);
		}


		void proxyDetection()
		{
			QFETCH(int, httpStatusCode);
			QFETCH(int, existingAppResult);
			const auto existingAppResultType = static_cast<UIPlugInWebService::ExistingAppResult>(existingAppResult);

			const auto testAddress = HttpServer::cAddresses.constFirst();
			QScopedValueRollback guard(HttpServer::cAddresses);

			MockNetworkManager networkManager;
			Env::set(NetworkManager::staticMetaObject, &networkManager);

			if (existingAppResultType == UIPlugInWebService::ExistingAppResult::SHOWUI_TIMEOUT)
			{
				QTest::ignoreMessage(QtWarningMsg, "ShowUI request timed out");
			}
			else
			{
				auto reply = new MockNetworkReply();
				reply->setAttribute(QNetworkRequest::Attribute::HttpStatusCodeAttribute, httpStatusCode);
				reply->fireFinished();
				networkManager.setNextReply(reply);

				if (existingAppResultType == UIPlugInWebService::ExistingAppResult::REBIND_FAILED)
				{
					HttpServer::cAddresses.clear();
				}
			}

			QCOMPARE(mUi->handleExistingApp(HttpServer::cPort, testAddress), existingAppResultType);
		}


		void test_GuessImageContentType_data()
		{
			QTest::addColumn<QString>("fileName");
			QTest::addColumn<QByteArray>("output");

			QTest::newRow("x-icon") << QString("test.ico") << QByteArrayLiteral("image/x-icon");
			QTest::newRow("X-ICON") << QString("test.iCo") << QByteArrayLiteral("image/x-icon");
			QTest::newRow("jpg") << QString("test.jpg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("JPG") << QString("test.JPG") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("jpeg") << QString("test.jpeg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("JPEG") << QString("test.Jpeg") << QByteArrayLiteral("image/jpeg");
			QTest::newRow("png") << QString("test.png") << QByteArrayLiteral("image/png");
			QTest::newRow("PNG") << QString("test.pNG") << QByteArrayLiteral("image/png");
			QTest::newRow("svg") << QString("test.svg") << QByteArrayLiteral("image/svg+xml");
			QTest::newRow("SVG") << QString("TEST.SVG") << QByteArrayLiteral("image/svg+xml");
			QTest::newRow("other") << QString("test.test") << QByteArrayLiteral("image");
		}


		void test_GuessImageContentType()
		{
			QFETCH(QString, fileName);
			QFETCH(QByteArray, output);

			QCOMPARE(mUi->guessImageContentType(fileName), output);
		}


};

QTEST_GUILESS_MAIN(test_UIPlugInWebService)
#include "test_UIPlugInWebService.moc"
