/*!
 * \brief Unit tests for \ref NetworkManager
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "NetworkManager.h"

#include "context/SelfAuthContext.h"
#include "controller/SelfAuthController.h"
#include "Env.h"
#include "LogHandler.h"
#include "SecureStorage.h"

#include "MockNetworkManager.h"
#include "MockNetworkReply.h"

#include <QtCore>
#include <QtNetwork>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QSharedPointer<GlobalStatus>)

class test_NetworkManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void paosRequestAttached()
		{
			QNetworkRequest request(QUrl("https://dummy"));
			auto reply = Env::getSingleton<NetworkManager>()->paos(request, "paosNamespace", "content", false, QByteArray(), 1);
			QVERIFY(request.hasRawHeader("PAOS"));
			QCOMPARE(request.rawHeader("PAOS"), QByteArray("ver=\"paosNamespace\""));
			QCOMPARE(reply->request(), request);
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 6);
			QVERIFY(request.sslConfiguration().ellipticCurves().contains(QSslEllipticCurve::fromLongName("prime256v1")));
			const auto cipherCount = Env::getSingleton<SecureStorage>()->getTlsConfig().getCiphers().size();
			QCOMPARE(request.sslConfiguration().ciphers().size(), cipherCount);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("ECDHE-RSA-AES256-GCM-SHA384")));
		}


		void paosRequestPsk()
		{
			QNetworkRequest request(QUrl("https://dummy"));
			auto reply = Env::getSingleton<NetworkManager>()->paos(request, "paosNamespace", "content", true, QByteArray(), 1);
			QVERIFY(request.hasRawHeader("PAOS"));
			QCOMPARE(request.rawHeader("PAOS"), QByteArray("ver=\"paosNamespace\""));
			QCOMPARE(reply->request(), request);
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 0);
			const auto cipherCount = Env::getSingleton<SecureStorage>()->getTlsConfig(SecureStorage::TlsSuite::PSK).getCiphers().size();
			QCOMPARE(request.sslConfiguration().ciphers().size(), cipherCount);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-CBC-SHA256")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-GCM-SHA256")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-CBC-SHA384")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-GCM-SHA384")));
		}


		void serviceUnavailableEnums()
		{
			auto reply = QSharedPointer<MockNetworkReply>::create();
			reply->setNetworkError(QNetworkReply::ServiceUnavailableError, "dummy error msg");

			QCOMPARE(NetworkManager::toNetworkError(reply), NetworkManager::NetworkError::ServiceUnavailable);
			QCOMPARE(NetworkManager::toTrustedChannelStatus(reply), GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, reply->url().toString()}));
			QCOMPARE(NetworkManager::toStatus(reply), GlobalStatus(GlobalStatus::Code::Network_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, reply->url().toString()}));
		}


		void serviceUnavailable_data()
		{
			QTest::addColumn<QSharedPointer<GlobalStatus> >("status");
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


		void serviceUnavailableWorkflow()
		{
			MockNetworkManager networkManager;
			Env::set(NetworkManager::staticMetaObject, &networkManager);
			connect(&networkManager, &MockNetworkManager::fireReply, this, [&] {
						networkManager.fireFinished();
					}, Qt::QueuedConnection);

			auto reply = new MockNetworkReply();
			reply->setNetworkError(QNetworkReply::ServiceUnavailableError, "dummy");
			networkManager.setNextReply(reply);

			auto context = QSharedPointer<SelfAuthContext>::create();
			connect(context.data(), &AuthContext::fireStateChanged, this, [&] {
						context->setStateApproved();
					});

			SelfAuthController controller(context);
			QSignalSpy spy(&controller, &WorkflowController::fireComplete);

			controller.run();

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(context->getStatus(), GlobalStatus(GlobalStatus::Code::Workflow_TrustedChannel_ServiceUnavailable, {GlobalStatus::ExternalInformation::LAST_URL, QString()}));
		}


};

QTEST_GUILESS_MAIN(test_NetworkManager)
#include "test_NetworkManager.moc"
