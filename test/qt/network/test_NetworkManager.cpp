/**
 * Copyright (c) 2014-2026 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkManager.h"

#include "Env.h"
#include "LogHandler.h"
#include "ReaderManager.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "context/SelfAuthContext.h"
#include "controller/SelfAuthController.h"

#include "MockNetworkManager.h"
#include "MockNetworkReply.h"

#include <QNetworkReply>
#include <QtCore>
#include <QtNetwork>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<GlobalStatus>)

class test_NetworkManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			Env::getSingleton<LogHandler>()->init();

			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void paosRequestAttached()
		{
			QNetworkRequest request(QUrl("https://dummy"_L1));
			auto reply = Env::getSingleton<NetworkManager>()->paos(request, "paosNamespace", "content", false, QByteArray());
			reply->abort();
			QVERIFY(request.hasRawHeader("PAOS"));
			QCOMPARE(request.rawHeader("PAOS"), QByteArray("ver=\"paosNamespace\""));
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 5);
			QVERIFY(request.sslConfiguration().ellipticCurves().contains(QSslEllipticCurve::fromLongName("prime256v1"_L1)));
			const auto cipherCount = Env::getSingleton<SecureStorage>()->getTlsConfig().getCiphers().size();
			QCOMPARE(request.sslConfiguration().ciphers().size(), cipherCount);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("ECDHE-RSA-AES256-GCM-SHA384"_L1)));
		}


		void paosRequestPsk()
		{
			QNetworkRequest request(QUrl("https://dummy"_L1));
			auto reply = Env::getSingleton<NetworkManager>()->paos(request, "paosNamespace", "content", true, QByteArray());
			reply->abort();
			QVERIFY(request.hasRawHeader("PAOS"));
			QCOMPARE(request.rawHeader("PAOS"), QByteArray("ver=\"paosNamespace\""));
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 0);
			const auto cipherCount = Env::getSingleton<SecureStorage>()->getTlsConfig(SecureStorage::TlsSuite::PSK).getCiphers().size();
			QCOMPARE(request.sslConfiguration().ciphers().size(), cipherCount);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-CBC-SHA256"_L1)));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-GCM-SHA256"_L1)));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-CBC-SHA384"_L1)));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-GCM-SHA384"_L1)));
		}


		void serviceUnavailableEnums()
		{
			auto reply = QSharedPointer<MockNetworkReply>::create();
			reply->setError(QNetworkReply::ServiceUnavailableError, "dummy error msg"_L1);

			QCOMPARE(NetworkManager::toNetworkError(reply), NetworkManager::NetworkError::ServiceUnavailable);
			QCOMPARE(NetworkManager::toTrustedChannelStatus(reply), GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, reply->url().toString()}));
			QCOMPARE(NetworkManager::toStatus(reply), GlobalStatus(GlobalStatus::Code::Network_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, reply->url().toString()}));
		}


		void serviceUnavailable_data()
		{
			QTest::addColumn<QSharedPointer<GlobalStatus>>("status");
			QTest::addColumn<bool>("param");
			QTest::addColumn<QString>("msg");

			const auto& msg = QStringLiteral("The service is temporarily not available. Please try again later.");

			auto status = QSharedPointer<GlobalStatus>::create(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable);
			QTest::newRow("trustedChannel") << status << true << msg;
			QTest::newRow("trustedChannel") << status << false << msg;

			status = QSharedPointer<GlobalStatus>::create(GlobalStatus::Code::Network_ServiceUnavailable);
			QTest::newRow("network") << status << true << msg;
			QTest::newRow("network") << status << false << msg;
		}


		void serviceUnavailable()
		{
			QFETCH(QSharedPointer<GlobalStatus>, status);
			QFETCH(bool, param);
			QFETCH(QString, msg);

			QCOMPARE(status->toErrorDescription(param), msg);
		}


		void test_onSslErrors_OcspNoResponseFound()
		{
			auto sessionTicket = QByteArray("sessionTicket");
			auto networkManager = Env::getSingleton<NetworkManager>();
			auto reply = QSharedPointer<MockNetworkReply>::create();
			auto spy = QSignalSpy(reply.data(), &QNetworkReply::finished);
			auto sslcfg = QSslConfiguration();

			sslcfg.setSessionTicket(sessionTicket);
			networkManager->mUpdaterSessions.insert(sessionTicket);
			reply->setSslConfigurationImplementation(sslcfg);
			networkManager->onSslErrors(reply, QList<QSslError>(1, {QSslError(QSslError::OcspNoResponseFound)}));
			QCOMPARE(spy.count(), 0);
		}


		void test_onSslErrors_CertificateBlacklisted()
		{
			auto networkManager = Env::getSingleton<NetworkManager>();
			auto reply = QSharedPointer<MockNetworkReply>::create();
			auto spy = QSignalSpy(reply.data(), &QNetworkReply::finished);
			auto sslcfg = QSslConfiguration();

			reply->setSslConfigurationImplementation(sslcfg);
			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression(QStringLiteral("Fatal SSL error: .*")));
			networkManager->onSslErrors(reply, QList<QSslError>(1, {QSslError(QSslError::CertificateBlacklisted)}));
			QCOMPARE(spy.count(), 1);
		}


		void test_onEncryptedResponse()
		{
			auto networkManager = Env::getSingleton<NetworkManager>();
			auto reply = QSharedPointer<MockNetworkReply>::create();
			auto spy = QSignalSpy(reply.data(), &QNetworkReply::finished);
			QTest::ignoreMessage(QtCriticalMsg, QRegularExpression(QStringLiteral("Untrusted certificate found .*")));
			networkManager->onEncryptedResponse(reply);
			QCOMPARE(spy.count(), 1);
		}


		void serviceUnavailableWorkflow()
		{
			MockNetworkManager networkManager;
			Env::set(NetworkManager::staticMetaObject, &networkManager);
			connect(&networkManager, &MockNetworkManager::fireReply, this, [&networkManager] {
						networkManager.fireFinished();
					}, Qt::QueuedConnection);

			auto* reply = new MockNetworkReply();
			const auto url = reply->url().toString();
			reply->setAttribute(QNetworkRequest::HttpStatusCodeAttribute, QVariant(503));
			reply->setError(QNetworkReply::ServiceUnavailableError, "dummy"_L1);
			networkManager.setNextReply(reply);

			auto context = QSharedPointer<SelfAuthContext>::create();
			connect(context.data(), &AuthContext::fireStateChanged, this, [&context] {
						context->setStateApproved();
					});

			SelfAuthController controller(context);
			QSignalSpy spy(&controller, &WorkflowController::fireComplete);

			controller.run();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			const GlobalStatus::ExternalInfoMap infoMap {
				{GlobalStatus::ExternalInformation::HTTP_STATUS_CODE, QString::number(503)},
				{GlobalStatus::ExternalInformation::LAST_URL, url}
			};
			QCOMPARE(context->getStatus(), GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, infoMap));
		}


		void closeConnectionsOnShutdown()
		{
			MockNetworkManager networkManager;
			Env::set(NetworkManager::staticMetaObject, &networkManager);

			auto reply1 = networkManager.trackConnection(new MockNetworkReply());
			QSignalSpy spy1(reply1.data(), &QNetworkReply::finished);

			QCOMPARE(networkManager.getOpenConnectionCount(), 1);

			auto reply2 = networkManager.trackConnection(new MockNetworkReply());
			QSignalSpy spy2(reply2.data(), &QNetworkReply::finished);
			QCOMPARE(networkManager.getOpenConnectionCount(), 2);

			static_cast<MockNetworkReply*>(reply1.get())->fireFinished();
			QTRY_COMPARE(spy1.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(networkManager.getOpenConnectionCount(), 1);

			Q_EMIT networkManager.fireShutdown();
			QTRY_COMPARE(spy2.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(networkManager.getOpenConnectionCount(), 0);
		}


};

QTEST_GUILESS_MAIN(test_NetworkManager)
#include "test_NetworkManager.moc"
