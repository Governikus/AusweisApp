/*!
 * \brief Unit tests for \ref WebserviceActivationHandler
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WebserviceActivationHandler.h"

#include "LogHandler.h"
#include "MockSocket.h"
#include "ResourceLoader.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;


class test_WebserviceActivationHandler
	: public QObject
{
	Q_OBJECT
	WebserviceActivationHandler mHandler;
	QPointer<MockSocket> mSocket;
	QSharedPointer<HttpRequest> mRequest;
	QScopedPointer<QSignalSpy> mShowUiSpy, mShowUserInfoSpy, mAuthenticationSpy;

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mSocket = new MockSocket();
			mRequest.reset(new HttpRequest(mSocket));
			mShowUiSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireShowUiRequest));
			mShowUserInfoSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireShowUserInformation));
			mAuthenticationSpy.reset(new QSignalSpy(&mHandler, &WebserviceActivationHandler::fireAuthenticationRequest));
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void showUI_noModule()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::DEFAULT));
		}


		void showUI_moduleCurrent()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI=current");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::CURRENT));
		}


		void showUI_modulePINManagement()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI=PINManagement");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::PINMANAGEMENT));
		}


		void showUI_modulePINManagement_caseInsensitive()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?showui=pinManagement");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::PINMANAGEMENT));
		}


		void showUI_moduleSettings()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI=Settings");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUiSpy->takeFirst().at(0).toInt(), static_cast<int>(UiModule::SETTINGS));
		}


		void status_noFormat()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?status");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QVERIFY(mSocket->mWriteBuffer.contains("Content-Type: text/plain; charset=utf-8"));
		}


		void status_formatPlain()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?status=plain");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QVERIFY(mSocket->mWriteBuffer.contains("Content-Type: text/plain; charset=utf-8"));
		}


		void status_formatJson()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?status=json");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QVERIFY(mSocket->mWriteBuffer.contains("Content-Type: application/json"));
		}


		void status_formatJson_caseInsensitive()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?status=JsoN");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 0);
			QVERIFY(mSocket->mWriteBuffer.contains("Content-Type: application/json"));
		}


		void authentication()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?tctokenURL=bla");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
			QCOMPARE(mAuthenticationSpy->count(), 1);
		}


		void unknownRequest()
		{
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?unknownRequest");

			mHandler.onNewRequest(mRequest);
			QVERIFY(mSocket->mWriteBuffer.contains("HTTP/1.0 404 Not Found"));
		}


		void sameUserAgentVersion()
		{
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI");
			mRequest->mHeader.insert("user-agent", QCoreApplication::applicationName().toUtf8() + '/' + QCoreApplication::applicationVersion().toUtf8() + " (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 1);
			QCOMPARE(mShowUserInfoSpy->count(), 0);
		}


		void currentHigherThanCallerUserAgent()
		{
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI");
			mRequest->mHeader.insert("user-agent", QCoreApplication::applicationName().toUtf8() + "/0.0.0 (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest);

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
			QSignalSpy spy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);
			QCoreApplication::setApplicationVersion("1.0.0");
			mRequest->mUrl = QByteArray("http://localhost:24727/eID-Client?ShowUI");
			mRequest->mHeader.insert("user-agent", QCoreApplication::applicationName().toUtf8() + "/2.0.0 (TR-03124-1/1.2)");

			mHandler.onNewRequest(mRequest);

			QCOMPARE(mAuthenticationSpy->count(), 0);
			QCOMPARE(mShowUiSpy->count(), 0);
			QCOMPARE(mShowUserInfoSpy->count(), 1);
			QVERIFY(mShowUserInfoSpy->takeFirst().at(0).toString().contains("You tried to start a newer version"));

			QVERIFY(spy.count() > 0);
			auto param = spy.takeLast();
			QVERIFY(param.at(0).toString().contains("Current version is lower than caller version"));
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

			QCOMPARE(mHandler.guessImageContentType(fileName), output);
		}


};

QTEST_GUILESS_MAIN(test_WebserviceActivationHandler)
#include "test_WebserviceActivationHandler.moc"
