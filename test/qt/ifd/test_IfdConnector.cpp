/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "IfdConnectorImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "KeyPair.h"
#include "RemoteWebSocketServer.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "TlsChecker.h"
#include "messages/Discovery.h"

#include "TestHookThread.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(Discovery)
Q_DECLARE_METATYPE(QSharedPointer<IfdDispatcher>)


class test_IfdConnector
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<TestHookThread> mClientThread;
		const KeyPair pair1 = KeyPair::generate();
		const KeyPair pair2 = KeyPair::generate();
		const KeyPair pair3 = KeyPair::generate();

		Discovery getDiscovery(const QString& pIfdName, quint16 pPort)
		{
			QByteArray ifdId("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF"_ba);
			IfdVersion::Version version = IfdVersion::Version::latest;

			if (IfdVersion::supported().contains(IfdVersion::Version::v2))
			{
				ifdId = R"(
							-----BEGIN CERTIFICATE-----
							MIIC4zCCAcsCBEQvMpowDQYJKoZIhvcNAQELBQAwNDEUMBIGA1UEAwwLQXVzd2Vp
							c0FwcDIxHDAaBgNVBAUTEzY0MTgwMjY3MTE5MTA5MjY2MzQwIhgPMTk3MDAxMDEw
							MDAwMDBaGA85OTk5MTIzMTIzNTk1OVowNDEUMBIGA1UEAwwLQXVzd2Vpc0FwcDIx
							HDAaBgNVBAUTEzY0MTgwMjY3MTE5MTA5MjY2MzQwggEiMA0GCSqGSIb3DQEBAQUA
							A4IBDwAwggEKAoIBAQDGJ9C76Cb8iHuaZJxcFY0NpNllcAK5JKcrigKBki7EvF9z
							5Q/MNek2pxwTMp5SilUDJOkwgcTdm7liC/Zx+lPX8MZjhWxV73DGt9DDyJh91ypl
							6B8vZbpJlL83Vo4C4BLBG6ZaElPpyTitWWKQ4BFUoH0h2utsNFV7FHz+1oZcvhv0
							gQuzd7gQaVV6mzCePRn+4qgxYSXSJ8Ix21udgT3LoHDOBrOWSIt0g/Q1mkzJcnaC
							EnS2s6Ib0xPY5PsL1YN/dZn88/gs9Za4rZSBGIIDrpCt5WCkYbkg45LwXLmaPUrg
							uuFIFIR0HH4pxgajLHpMgYaszxkg4SkdxwJ8vIytAgMBAAEwDQYJKoZIhvcNAQEL
							BQADggEBAB4grwHZ8NMrs3vRInJQc3ftYDCAjDPjjTg/G4BVz07DmlQZpFyPfInb
							UaKfpMlaEd1EoRuNIxC796+rZhy+j97DoLkT1qnPP30GLwlZaVZeOCKIIQ+tGjUU
							cWhhIC6kRCPIQAKxKDNGIUwBAkwludieGa7Ytl7qmnnJDMNe+Ox7Sf+UOa12bJKH
							d27MYoWMfecJdTmF8xXQ7EEYjMwWHd5t5tJG9AVgzhO8zC+iJTqc9I34sIa8+9WE
							oQu+/VZgDkJaSdDJ4LqVFIvUy3CFGh6ahDVsHGC5kTDm5EQWh3puWR0AkIjUWMPi
							xU/nr0Jsab99VgX4/nnCW92v/DIRc1c=
							-----END CERTIFICATE-----
						)"_ba;
				version = IfdVersion::Version::v2;
			}
			Discovery discovery(pIfdName, ifdId, pPort, {version});
			discovery.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			return discovery;
		}


		void sendRequest(const QSharedPointer<IfdConnectorImpl>& pConnector,
			const Discovery& pDiscovery,
			const QByteArray& pPassword)
		{
			QMetaObject::invokeMethod(pConnector.data(), [ = ] {
						pConnector->onConnectRequest(pDiscovery, pPassword);
					}, Qt::QueuedConnection);
		}


		void verifySuccessSignal(const QSignalSpy& pSignalSpy,
			const QByteArray& pIfdId)
		{
			bool signalFound = false;
			for (const QList<QVariant>& arguments : pSignalSpy)
			{
				const QVariant remoteDeviceDescriptorVariant = arguments.at(0);
				QVERIFY(remoteDeviceDescriptorVariant.canConvert<QByteArray>());
				const auto& ifdId = remoteDeviceDescriptorVariant.value<QByteArray>();

				const QVariant dispatcherVariant = arguments.at(1);
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<IfdDispatcherClient>>());
				const QSharedPointer<IfdDispatcherClient> dispatcher = dispatcherVariant.value<QSharedPointer<IfdDispatcherClient>>();
				QVERIFY(dispatcher);

				if (ifdId == pIfdId)
				{
					qDebug() << "Success verified";
					signalFound = true;
					break;
				}
			}
			QVERIFY(signalFound);
		}


		void verifyErrorSignal(const QSignalSpy& pSignalSpy,
			const QList<IfdErrorCode>& pRemoteErrorCodeList,
			const QByteArray& pIfdId,
			const bool pExpectingNullDeviceDescriptor = false)
		{
			bool signalFound = false;
			for (const QList<QVariant>& arguments : pSignalSpy)
			{
				const QVariant remoteDeviceDescriptorVariant = arguments.at(0);
				QVERIFY(remoteDeviceDescriptorVariant.canConvert<QByteArray>());
				const auto& ifdId = remoteDeviceDescriptorVariant.value<QByteArray>();

				if (pExpectingNullDeviceDescriptor)
				{
					qDebug() << "Error verified";
					signalFound = true;
					break;
				}

				const QVariant errorCodeVariant = arguments.at(1);
				QVERIFY(errorCodeVariant.canConvert<IfdErrorCode>());
				const auto errorCode = errorCodeVariant.value<IfdErrorCode>();
				if (ifdId == pIfdId && pRemoteErrorCodeList.contains(errorCode))
				{
					qDebug() << "Error verified";
					signalFound = true;
					break;
				}
			}
			QVERIFY(signalFound);
		}

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			qRegisterMetaType<QWebSocketProtocol::CloseCode>("QWebSocketProtocol::CloseCode");
			Env::getSingleton<AppSettings>(); // just init in MainThread because of QObject
			QSslSocket().setSocketDescriptor(0); // init Q_GLOBAL_STATIC(QSocks5BindStore, socks5BindStore)
		}


		void init()
		{
			mClientThread.reset(new TestHookThread());
		}


		void cleanup()
		{
			mClientThread.reset();
		}


		void requestWithNoDeviceNameIsRejected()
		{
			const QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spySuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// No device name.
			Discovery discoveryMsg(QString(), QByteArrayLiteral("0123456789ABCDEF"), 2020, {IfdVersion::Version::latest});
			discoveryMsg.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			sendRequest(connector, discoveryMsg, QByteArray());
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyError, {IfdErrorCode::INVALID_REQUEST}, discoveryMsg.getIfdId());
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithNoDeviceInfoIsRejected()
		{
			const QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spySuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// Device information is null.
			Discovery discoveryMsg((QJsonObject()));
			discoveryMsg.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			sendRequest(connector, Discovery(QJsonObject()), "secret");
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyError, {IfdErrorCode::INVALID_REQUEST}, discoveryMsg.getIfdId(), true);
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithEncryptedServerAndReconnectFails()
		{
			QScopedPointer<RemoteWebSocketServer> server(Env::create<RemoteWebSocketServer*>());
			server->listen("dummy"_L1);

			QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyDestroy(connector.data(), &IfdConnector::destroyed);
			QSignalSpy spyError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spySuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// Password is empty.
			const auto& discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), server->getServerPort());
			QTest::ignoreMessage(QtMsgType::QtDebugMsg, "Request connection.");

			sendRequest(connector, discoveryMsg, QByteArray());
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyError, {IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION}, discoveryMsg.getIfdId());
			QCOMPARE(spySuccess.count(), 0);

			connector.reset();
			QTRY_COMPARE(spyDestroy.size(), 1);
		}


		void requestWithUnsupportedApiLevelIsRejected()
		{
			const QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spySuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// Currently, only API level 1 is supported.
			Discovery discoveryMsg(QStringLiteral("Smartphone1"), QByteArrayLiteral("0123456789ABCDEF"), 2020, {IfdVersion::Version::Unknown});
			discoveryMsg.setAddresses({QHostAddress(QHostAddress::LocalHost)});
			sendRequest(connector, discoveryMsg, "secret");
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyError, {IfdErrorCode::NO_SUPPORTED_API_LEVEL}, discoveryMsg.getIfdId());
			QCOMPARE(spySuccess.count(), 0);
		}


		void noServerResponseCausesConnectionErrorOrTimeout()
		{
			const QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(2500), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spySuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// Correct request but no server is running.
			const auto& discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), 2020);
			QTest::ignoreMessage(QtMsgType::QtDebugMsg, "Request connection.");
			sendRequest(connector, discoveryMsg, "dummy");
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyError, {IfdErrorCode::CONNECTION_ERROR}, discoveryMsg.getIfdId());
			QCOMPARE(spySuccess.count(), 0);
		}


		void encryptedConnectionSucceeds_data()
		{
			QTest::addColumn<QByteArray>("psk");
			QTest::addColumn<QSslConfiguration>("sslConfigServer");
			QTest::addColumn<QList<QSslCertificate>>("trustedCertificates");

			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			QVERIFY(settings.checkAndGenerateKey(3072));

			QVERIFY(pair1.isValid());

			QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setPrivateKey(pair1.getKey());
			config.setLocalCertificate(pair1.getCertificate());
			config.setCaCertificates({pair2.getCertificate(), TlsChecker::getRootCertificate(settings.getCertificates()), pair3.getCertificate()});
			QTest::newRow("paired") << QByteArray() << config << QList<QSslCertificate>({pair2.getCertificate(), pair1.getCertificate(), pair3.getCertificate()});

			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair1.getKey());
			config.setLocalCertificate(pair1.getCertificate());
			QTest::newRow("unpaired") << "123456"_ba << config << QList<QSslCertificate>();
		}


		void encryptedConnectionSucceeds()
		{
			QFETCH(QByteArray, psk);
			QFETCH(QSslConfiguration, sslConfigServer);
			QFETCH(QList<QSslCertificate>, trustedCertificates);

			QWebSocketServer webSocketServer(QStringLiteral("Smartphone1"), QWebSocketServer::SecureMode);
			connect(&webSocketServer, &QWebSocketServer::preSharedKeyAuthenticationRequired, this, [&psk](QSslPreSharedKeyAuthenticator* pAuthenticator){
						pAuthenticator->setPreSharedKey(psk);
					});
			QSignalSpy spySocketError(&webSocketServer, &QWebSocketServer::serverError);
			QSignalSpy spySocketSuccess(&webSocketServer, &QWebSocketServer::newConnection);

			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates(trustedCertificates);
			webSocketServer.setSslConfiguration(sslConfigServer);

			// Listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
			webSocketServer.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
			webSocketServer.listen(QHostAddress(QHostAddress::LocalHost));
			const quint16 serverPort = webSocketServer.serverPort();

			// Execute test in internal block so that all relevant smart pointers are released before stopping the client thread.
			{
				QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
				connector->moveToThread(mClientThread->getThread());

				QSignalSpy spyConnectorDestroyed(connector.data(), &IfdConnector::destroyed);
				QSignalSpy spyConnectorError(connector.data(), &IfdConnector::fireDispatcherError);
				QSignalSpy spyConnectorSuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

				// Send valid encrypted connect request.
				const auto& discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), serverPort);
				QTest::ignoreMessage(QtMsgType::QtDebugMsg, "Request connection.");
				sendRequest(connector, discoveryMsg, psk);
				QTRY_COMPARE(spyConnectorSuccess.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyConnectorError.count(), 0);
				QCOMPARE(connector->mPendingRequests.size(), 0);
				verifySuccessSignal(spyConnectorSuccess, discoveryMsg.getIfdId());

				QVariant dispatcherVariant = spyConnectorSuccess.takeFirst().at(1);
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<IfdDispatcherClient>>());
				dispatcherVariant.clear();

				connector.reset();
				QTRY_COMPARE(spyConnectorDestroyed.size(), 1);
			}

			QCOMPARE(spySocketError.count(), 0);
			QCOMPARE(spySocketSuccess.count(), 1);

			QSignalSpy spyClosed(&webSocketServer, &QWebSocketServer::closed);
			webSocketServer.close();
			QTRY_COMPARE(spyClosed.size(), 1);
		}


		void encryptedConnectionWithWrongPasswordFails()
		{
			QWebSocketServer webSocketServer(QStringLiteral("Smartphone1"), QWebSocketServer::SecureMode);
			QObject::connect(&webSocketServer, &QWebSocketServer::preSharedKeyAuthenticationRequired, this, [](QSslPreSharedKeyAuthenticator* pAuthenticator){
						pAuthenticator->setPreSharedKey(QByteArray("secret"));
					});
			QSignalSpy spySocketError(&webSocketServer, &QWebSocketServer::serverError);
			QSignalSpy spySocketSuccess(&webSocketServer, &QWebSocketServer::newConnection);

			QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setPrivateKey(pair1.getKey());
			config.setLocalCertificate(pair1.getCertificate());
			webSocketServer.setSslConfiguration(config);

			// Listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
			webSocketServer.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
			webSocketServer.listen(QHostAddress(QHostAddress::LocalHost));
			const quint16 serverPort = webSocketServer.serverPort();

			QSharedPointer<IfdConnectorImpl> connector(new IfdConnectorImpl(), &IfdConnectorImpl::deleteLater);
			connector->moveToThread(mClientThread->getThread());

			QSignalSpy spyConnectorDestroyed(connector.data(), &IfdConnector::destroyed);
			QSignalSpy spyConnectorError(connector.data(), &IfdConnector::fireDispatcherError);
			QSignalSpy spyConnectorSuccess(connector.data(), &IfdConnector::fireDispatcherCreated);

			// Send encrypted connect request with wrong psk.
			const auto& discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), serverPort);
			QTest::ignoreMessage(QtMsgType::QtDebugMsg, "Request connection.");
			sendRequest(connector, discoveryMsg, "sekret");

			QTRY_COMPARE(spyConnectorError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyConnectorSuccess.count(), 0);
			QCOMPARE(connector->mPendingRequests.size(), 0);

			verifyErrorSignal(spyConnectorError, {IfdErrorCode::REMOTE_HOST_REFUSED_CONNECTION}, discoveryMsg.getIfdId());

			QCOMPARE(spySocketError.count(), 0);
			QCOMPARE(spySocketSuccess.count(), 0);

			connector.reset();
			QTRY_COMPARE(spyConnectorDestroyed.size(), 1);
		}


};

QTEST_GUILESS_MAIN(test_IfdConnector)
#include "test_IfdConnector.moc"
