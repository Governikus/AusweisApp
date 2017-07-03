/*!
 * test_WebserviceActivationHandler.cpp
 * \brief Unit tests for \ref WebserviceActivationHandler
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "WebserviceActivationHandler.h"

#include "LogHandler.h"
#include "MockQHttpConnection.h"
#include "ResourceLoader.h"
#include "qhttpserver/qhttprequest.h"
#include "qhttpserver/qhttpresponse.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_WebserviceActivationHandler
	: public QObject
{
	Q_OBJECT
	QSharedPointer<MockQHttpConnection> mConnection;
	WebserviceActivationHandler mHandler;
	QSharedPointer<QHttpRequest> mRequest;
	QSharedPointer<QHttpResponse> mResponse;
	QScopedPointer<QSignalSpy> mShowUiSpy, mShowUserInfoSpy, mAuthenticationSpy;

	private Q_SLOTS:
		void initTestCase()
		{
			LogHandler::getInstance().init();
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mConnection.reset(new MockQHttpConnection());
			mRequest.reset(new QHttpRequest());
			mResponse.reset(new QHttpResponse(mConnection));
			mShowUiSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireShowUiRequest));
			mShowUserInfoSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireShowUserInformation));
			mAuthenticationSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireAuthenticationRequest));
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void showUI_noModule()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::DEFAULT));
		}


		void showUI_moduleCurrent()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI=current"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::CURRENT));
		}


		void showUI_modulePINManagement()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI=PINManagement"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::PINMANAGEMENT));
		}


		void showUI_modulePINManagement_caseInsensitive()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?showui=pinManagement"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::PINMANAGEMENT));
		}


		void showUI_moduleSettings()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI=Settings"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::SETTINGS));
		}


		void status_noFormat()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?status"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mResponse->m_headers.value("Content-Type"), QString("text/plain; charset=utf-8"));
		}


		void status_formatPlain()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?status=plain"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mResponse->m_headers.value("Content-Type"), QString("text/plain; charset=utf-8"));
		}


		void status_formatJson()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?status=json"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mResponse->m_headers.value("Content-Type"), QString("application/json"));
		}


		void status_formatJson_caseInsensitive()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?status=JsoN"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mResponse->m_headers.value("Content-Type"), QString("application/json"));
		}


		void authentication()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?tctokenURL=bla"));

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void unknownRequest()
		{
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?unknownRequest"));

			mHandler.onNewRequest(mRequest, mResponse);

			QVERIFY(mConnection->mContent.contains("HTTP/1.1 404 Not Found"));
		}


		void sameUserAgentVersion()
		{
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI"));
			mRequest->m_headers.insert("user-agent", QCoreApplication::applicationName() + "/" + QCoreApplication::applicationVersion() + " (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
		}


		void currentHigherThanCallerUserAgent()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI"));
			mRequest->m_headers.insert("user-agent", QCoreApplication::applicationName() + "/0.0.0 (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start an older version"));

			QVERIFY(spy.count() > 0);
			auto param = spy.takeLast();
			QVERIFY(param.at(0).toString().contains("Current version is higher than caller version"));
		}


		void currentLowerThanCallerUserAgent()
		{
			QSignalSpy spy(&LogHandler::getInstance(), &LogHandler::fireLog);
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->setUrl(QUrl("http://localhost:24727/eID-Client?ShowUI"));
			mRequest->m_headers.insert("user-agent", QCoreApplication::applicationName() + "/2.0.0 (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest, mResponse);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start a newer version"));

			QVERIFY(spy.count() > 0);
			auto param = spy.takeLast();
			QVERIFY(param.at(0).toString().contains("Current version is lower than caller version"));
		}


};

QTEST_GUILESS_MAIN(test_WebserviceActivationHandler)
#include "test_WebserviceActivationHandler.moc"
