/*!
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "states/StateSendWhitelistSurvey.h"

#include "AppSettings.h"
#include "HttpServer.h"
#include "LogHandler.h"
#include "states/StateBuilder.h"
#include "SecureStorage.h"
#include "VolatileSettings.h"

#include "MockCardConnectionWorker.h"
#include "TestAuthContext.h"

#include <QtTest>


using namespace governikus;

class MockSecureStorage
	: public SecureStorage
{
	Q_GADGET

	public:
		explicit MockSecureStorage(const QUrl& url)
			: SecureStorage()
		{
			mWhitelistServerBaseUrl = url;
		}


};


class test_StateSendWhitelistSurvey
	: public QObject
{
	Q_OBJECT
	QSharedPointer<StateSendWhitelistSurvey> mState;
	QSharedPointer<TestAuthContext> mContext;

	private Q_SLOTS:
		void init()
		{
			mContext.reset(new TestAuthContext(nullptr, ":/paos/DIDAuthenticateEAC1.xml"));
			mState.reset(StateBuilder::createState<StateSendWhitelistSurvey>(mContext));
			mState->onEntry(nullptr);
		}


		void cleanup()
		{
			mState.clear();
			mContext.clear();
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void test_Run_NoSurveyPending()
		{
			SDK_MODE(false);
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(false);
			QSignalSpy spy(mState.data(), &StateSendWhitelistSurvey::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "No survey pending.");
			mContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_Run_AsSDK()
		{
			SDK_MODE(true);
			QSignalSpy spy(mState.data(), &StateSendWhitelistSurvey::fireContinue);

			QTest::ignoreMessage(QtDebugMsg, "Running as SDK. Ignoring whitelist survey.");
			mContext->setStateApproved();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void test_Run_StatusIsError()
		{
			// Implementation at this time not possible.
			// Q_ASSERT handling unsupported.
		}


		void test_Run_EffectiveAccessRightsEmpty()
		{
			// Implementation at this time not possible.
			// Q_ASSERT handling unsupported.
		}


		void test_Run_DidAuthenticateEac1IsNullptr()
		{
			// Implementation at this time not possible.
			// Q_ASSERT handling unsupported.
		}


		void test_Run_data()
		{
			QTest::addColumn<quint16>("port");
			QTest::addColumn<int>("spyCounter");
			QTest::addColumn<QUrl>("url");

			QTest::newRow("successful") << quint16(25000) << 1 << QUrl("http://localhost:25000");
			QTest::newRow("invalidPort") << quint16(1515) << 0 << QUrl("http://localhost:25000");
			QTest::newRow("emptyPort") << quint16() << 0 << QUrl("http://localhost:25000");
			QTest::newRow("emptyPortinUrl") << quint16(1515) << 0 << QUrl("http://localhost");
			QTest::newRow("invalidHost") << quint16(25000) << 0 << QUrl("http://test:25000");
		}


		void test_Run()
		{
			SDK_MODE(false);
			QFETCH(quint16, port);
			QFETCH(int, spyCounter);
			QFETCH(QUrl, url);

			Env::getSingleton<LogHandler>()->init();
			Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(true);
			QSignalSpy spyContinue(mState.data(), &StateSendWhitelistSurvey::fireContinue);
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			HttpServer server(port);
			QSignalSpy spy(&server, &HttpServer::fireNewHttpRequest);

			const QString name("reader");
			mContext->setReaderName(name);
			MockSecureStorage storage(url);
			Env::set(SecureStorage::staticMetaObject, &storage);

			mContext->setStateApproved();
			QTRY_COMPARE(spy.count(), spyCounter); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(spyContinue.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(!Env::getSingleton<AppSettings>()->getGeneralSettings().isDeviceSurveyPending());

			bool surveySent = false;
			QString message;
			for (int i = 0; i < logSpy.size(); i++)
			{
				if (logSpy.at(i).at(0).toString().contains("Sent survey to whitelist server:"))
				{
					surveySent = true;
					message = logSpy.at(i).at(0).toString();
					break;
				}
			}

			QVERIFY(surveySent);
			QVERIFY(message.contains("AusweisAppVersionNumber"));
			QVERIFY(message.contains("ModelName"));
			QVERIFY(message.contains("ModelNumber"));
			QVERIFY(message.contains("Rom"));
			QVERIFY(message.contains("AndroidVersion"));
			QVERIFY(message.contains("BuildNumber"));
			QVERIFY(message.contains("KernelVersion"));
			QVERIFY(message.contains("\"MaximumNfcPacketLength\": 0"));
			QVERIFY(message.contains("Vendor"));

			if (spyCounter != 0)
			{
				const QSharedPointer<HttpRequest> request = spy.at(0).at(0).value<QSharedPointer<HttpRequest> >();
				QCOMPARE(request->getUrl(), QUrl("/new"));
				QCOMPARE(request->getHeader(QByteArray("host")), QByteArray("localhost:25000"));
				QCOMPARE(request->getHeader(QByteArray("content-type")), QByteArray("application/json; charset=UTF-8"));
				QJsonDocument json = QJsonDocument::fromJson(request->getBody());
				QJsonObject jsonObject = json.object();
				QVERIFY(jsonObject.contains("AusweisAppVersionNumber"));
				QVERIFY(jsonObject.contains("ModelName"));
				QVERIFY(jsonObject.contains("ModelNumber"));
				QVERIFY(jsonObject.contains("Rom"));
				QVERIFY(jsonObject.contains("Vendor"));
			}
		}


};

QTEST_GUILESS_MAIN(test_StateSendWhitelistSurvey)
#include "test_StateSendWhitelistSurvey.moc"
