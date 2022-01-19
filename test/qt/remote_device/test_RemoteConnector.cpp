/*!
 * \brief Unit tests for \ref RemoteConnector
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteConnectorImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "KeyPair.h"
#include "messages/Discovery.h"
#include "RemoteWebSocketServer.h"
#include "SecureStorage.h"

#include <QtTest>
#include <QWebSocket>
#include <QWebSocketServer>

#include <functional>

using namespace governikus;

Q_DECLARE_METATYPE(RemoteDeviceDescriptor)
Q_DECLARE_METATYPE(QSharedPointer<RemoteDispatcher>)
Q_DECLARE_METATYPE(RemoteErrorCode)


class test_RemoteConnector
	: public QObject
{
	Q_OBJECT

	private:
		Discovery getDiscovery(const QString& pIfdName, quint16 pPort)
		{
			QString ifdId("0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF0123456789ABCDEF");
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
						)";
				version = IfdVersion::Version::v2;
			}
			return Discovery(pIfdName, ifdId, pPort, {version});
		}


		void sendRequest(const QSharedPointer<RemoteConnectorImpl>& pConnector,
			const QHostAddress& pHostAddress,
			const Discovery& pDiscovery,
			const QString& pPassword)
		{
			const RemoteDeviceDescriptor descr(pDiscovery, pHostAddress);
			QMetaObject::invokeMethod(pConnector.data(), [ = ] {
					pConnector->onConnectRequest(descr, pPassword);
				}, Qt::QueuedConnection);
		}


		void verifySuccessSignal(const QSignalSpy& pSignalSpy,
			const quint16 pPort)
		{
			const static QHostAddress HOST_ADDRESS(QHostAddress::LocalHost);
			const static QString IFD_NAME("Smartphone1");

			bool signalFound = false;
			for (const QList<QVariant>& arguments : pSignalSpy)
			{
				const QVariant remoteDeviceDescriptorVariant = arguments.at(0);
				QVERIFY(remoteDeviceDescriptorVariant.canConvert<RemoteDeviceDescriptor>());
				const RemoteDeviceDescriptor descr = remoteDeviceDescriptorVariant.value<RemoteDeviceDescriptor>();
				QVERIFY(!descr.isNull());

				const QVariant dispatcherVariant = arguments.at(1);
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<RemoteDispatcherClient> >());
				const QSharedPointer<RemoteDispatcherClient> dispatcher = dispatcherVariant.value<QSharedPointer<RemoteDispatcherClient> >();
				QVERIFY(dispatcher);

				const QUrl remoteUrl = descr.getUrl();
				const QString remoteAddress = remoteUrl.host();
				const int remotePort = remoteUrl.port();
				const bool signalMatches = remoteAddress == HOST_ADDRESS.toString() && remotePort == pPort &&
						descr.getIfdName() == IFD_NAME;
				if (signalMatches)
				{
					qDebug() << "Success verified";
					signalFound = true;
					break;
				}
			}
			QVERIFY(signalFound);
		}


		void verifyErrorSignal(const QSignalSpy& pSignalSpy,
			const QList<RemoteErrorCode>& pRemoteErrorCodeList,
			const quint16 pPort,
			const QString& pIfdName,
			const bool pExpectingNullDeviceDescriptor = false)
		{
			const static QHostAddress HOST_ADDRESS(QHostAddress::LocalHost);

			bool signalFound = false;
			for (const QList<QVariant>& arguments : pSignalSpy)
			{
				const QVariant remoteDeviceDescriptorVariant = arguments.at(0);
				QVERIFY(remoteDeviceDescriptorVariant.canConvert<RemoteDeviceDescriptor>());
				const RemoteDeviceDescriptor descr = remoteDeviceDescriptorVariant.value<RemoteDeviceDescriptor>();

				if (pExpectingNullDeviceDescriptor && descr.isNull())
				{
					qDebug() << "Error verified";
					signalFound = true;
					break;
				}
				QVERIFY(!descr.isNull());

				const QVariant errorCodeVariant = arguments.at(1);
				QVERIFY(errorCodeVariant.canConvert<RemoteErrorCode>());
				const auto errorCode = errorCodeVariant.value<RemoteErrorCode>();

				const QUrl remoteUrl = descr.getUrl();
				const QString remoteAddress = remoteUrl.host();
				const int remotePort = remoteUrl.port();
				const bool signalMatches = remoteAddress == HOST_ADDRESS.toString() &&
						remotePort == pPort &&
						descr.getIfdName() == pIfdName &&
						pRemoteErrorCodeList.contains(errorCode);
				if (signalMatches)
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
			qRegisterMetaType<QWebSocketProtocol::CloseCode>("QWebSocketProtocol::CloseCode");
		}


		void requestWithNoDeviceNameIsRejected()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// No device name.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const Discovery discoveryMsg(QString(), QStringLiteral("0123456789ABCDEF"), 2020, {IfdVersion::Version::latest});
			sendRequest(connector, hostAddress, discoveryMsg, QString());
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::INVALID_REQUEST}, 2020, QString());
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithNoDeviceInfoIsRejected()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Device information is null.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			sendRequest(connector, hostAddress, Discovery(QJsonObject()), QStringLiteral("secret"));
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::INVALID_REQUEST}, 0, QString(), true);
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithEncryptedServerAndReconnectFails()
		{
			QScopedPointer<RemoteWebSocketServer> server(Env::create<RemoteWebSocketServer*>());
			server->listen("dummy");
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Password is empty.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const Discovery discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), server->getServerPort());
			sendRequest(connector, hostAddress, discoveryMsg, QString());
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION}, server->getServerPort(), QStringLiteral("Smartphone1"));
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithUnsupportedApiLevelIsRejected()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Currently, only API level 1 is supported.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const Discovery discoveryMsg(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), 2020, {IfdVersion::Version::Unknown});
			sendRequest(connector, hostAddress, discoveryMsg, QStringLiteral("secret"));
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::NO_SUPPORTED_API_LEVEL}, 2020, QStringLiteral("Smartphone1"));
			QCOMPARE(spySuccess.count(), 0);
		}


		void noServerResponseCausesConnectionErrorOrTimeout()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(2500));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Correct request but no server is running.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const Discovery discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), 2020);
			sendRequest(connector, hostAddress, discoveryMsg, QString("dummy"));
			QTRY_COMPARE(spyError.count(), 1); // clazy:exclude=qstring-allocations

			clientThread.exit();
			QVERIFY(clientThread.wait());

#if !defined(Q_OS_FREEBSD) && !defined(Q_OS_WIN)
			verifyErrorSignal(spyError, {RemoteErrorCode::CONNECTION_ERROR}, 2020, QStringLiteral("Smartphone1"));
#else
			verifyErrorSignal(spyError, {RemoteErrorCode::CONNECTION_TIMEOUT, RemoteErrorCode::CONNECTION_ERROR}, 2020, QStringLiteral("Smartphone1"));
#endif

			QCOMPARE(spySuccess.count(), 0);
		}


		void encryptedConnectionSucceeds_data()
		{
			QTest::addColumn<QString>("psk");
			QTest::addColumn<QSslConfiguration>("sslConfigServer");
			QTest::addColumn<QList<QSslCertificate> >("trustedCertificates");

			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			QVERIFY(settings.checkAndGenerateKey());

			const KeyPair pair = KeyPair::generate();
			QVERIFY(pair.isValid());

			QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setCaCertificates({KeyPair::generate().getCertificate(), settings.getCertificate(), KeyPair::generate().getCertificate()});
			QTest::newRow("paired") << QString() << config << QList<QSslCertificate>({KeyPair::generate().getCertificate(), pair.getCertificate(), KeyPair::generate().getCertificate()});

			config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			QTest::newRow("unpaired") << QString("123456") << config << QList<QSslCertificate>();
		}


		void encryptedConnectionSucceeds()
		{
			QFETCH(QString, psk);
			QFETCH(QSslConfiguration, sslConfigServer);
			QFETCH(QList<QSslCertificate>, trustedCertificates);

			QWebSocketServer webSocketServer(QStringLiteral("Smartphone1"), QWebSocketServer::SecureMode);
			connect(&webSocketServer, &QWebSocketServer::preSharedKeyAuthenticationRequired, this, [&](QSslPreSharedKeyAuthenticator* pAuthenticator){
					pAuthenticator->setPreSharedKey(psk.toLatin1());
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

			// Set up client thread.
			QThread clientThread;

			// Execute test in internal block so that all relevant smart pointers are released before stopping the client thread.
			QSharedPointer<QSignalSpy> remoteDispatcherDestructionSpy;
			{
				const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
				QSignalSpy spyConnectorError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
				QSignalSpy spyConnectorSuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

				connector->moveToThread(&clientThread);
				clientThread.start();

				// Send valid encrypted connect request.
				const QHostAddress hostAddress(QHostAddress::LocalHost);
				const Discovery discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), serverPort);
				sendRequest(connector, hostAddress, discoveryMsg, psk);

				QTRY_COMPARE(spyConnectorSuccess.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(spyConnectorError.count(), 0);
				verifySuccessSignal(spyConnectorSuccess, serverPort);

				const QVariant dispatcherVariant = spyConnectorSuccess.first().at(1);
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<RemoteDispatcherClient> >());
				const QSharedPointer<RemoteDispatcherClient> dispatcher = dispatcherVariant.value<QSharedPointer<RemoteDispatcherClient> >();
				remoteDispatcherDestructionSpy.reset(new QSignalSpy(dispatcher.data(), &QObject::destroyed));
			}

			QCOMPARE(spySocketError.count(), 0);
			QCOMPARE(spySocketSuccess.count(), 1);

			QTRY_COMPARE(remoteDispatcherDestructionSpy->count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(remoteDispatcherDestructionSpy->count(), 1);

			clientThread.exit();
			QVERIFY(clientThread.wait());

			// Make sure that no pending web socket events are in the event queue when the test completes.
			// Pending events were occasionally delivered after the web socket had been destroyed, leading to a crash.
			QCoreApplication::processEvents();
		}


		void encryptedConnectionWithWrongPasswordFails()
		{
			QWebSocketServer webSocketServer(QStringLiteral("Smartphone1"), QWebSocketServer::SecureMode);
			QObject::connect(&webSocketServer, &QWebSocketServer::preSharedKeyAuthenticationRequired, [](QSslPreSharedKeyAuthenticator* pAuthenticator){
					pAuthenticator->setPreSharedKey(QByteArray("secret"));
				});
			QSignalSpy spySocketError(&webSocketServer, &QWebSocketServer::serverError);
			QSignalSpy spySocketSuccess(&webSocketServer, &QWebSocketServer::newConnection);

			QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote().getConfiguration();
			const KeyPair pair = KeyPair::generate();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			webSocketServer.setSslConfiguration(config);

			// Listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
			webSocketServer.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
			webSocketServer.listen(QHostAddress(QHostAddress::LocalHost));
			const quint16 serverPort = webSocketServer.serverPort();

			// Set up client thread.
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl());
			QSignalSpy spyConnectorError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spyConnectorSuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Send encrypted connect request with wrong psk.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const Discovery discoveryMsg = getDiscovery(QStringLiteral("Smartphone1"), serverPort);
			sendRequest(connector, hostAddress, discoveryMsg, QStringLiteral("sekret"));

			QTRY_COMPARE(spyConnectorError.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(spyConnectorSuccess.count(), 0);
			verifyErrorSignal(spyConnectorError, {RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION}, serverPort, QStringLiteral("Smartphone1"));

			QCOMPARE(spySocketError.count(), 0);
			QCOMPARE(spySocketSuccess.count(), 0);

			clientThread.exit();
			QVERIFY(clientThread.wait());
		}


};

QTEST_GUILESS_MAIN(test_RemoteConnector)
#include "test_RemoteConnector.moc"
