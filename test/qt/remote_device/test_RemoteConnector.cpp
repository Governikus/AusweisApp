/*!
 * \brief Unit tests for \ref RemoteConnector
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteConnectorImpl.h"

#include "AppSettings.h"
#include "Env.h"
#include "KeyPair.h"
#include "messages/Discovery.h"
#include "MockDataChannel.h"
#include "RemoteHelper.h"
#include "RemoteWebSocketServer.h"
#include "SecureStorage.h"

#include <QtTest/QtTest>
#include <QWebSocket>
#include <QWebSocketServer>

#include <functional>

using namespace governikus;

Q_DECLARE_METATYPE(RemoteDeviceDescriptor)
Q_DECLARE_METATYPE(QSharedPointer<RemoteDispatcher> )
Q_DECLARE_METATYPE(RemoteErrorCode)


class test_RemoteConnector
	: public QObject
{
	Q_OBJECT

	private:
		static const int cConnectTimeoutMs = 500;
		static const int cSignalTimeoutMs = 10 * cConnectTimeoutMs;

		void waitForSignals(QSignalSpy* const pSpy, const int pExpectedCount, const int pTimeoutMs)
		{
			for (int tryCount = 0; pSpy->count() < pExpectedCount && tryCount < pExpectedCount; ++tryCount)
			{
				pSpy->wait(pTimeoutMs);
			}
			QCOMPARE(pSpy->count(), pExpectedCount);
		}


		void sendRequest(const QSharedPointer<RemoteConnectorImpl>& pConnector,
			const QHostAddress& pHostAddress,
			const QSharedPointer<const Discovery>& pDiscovery,
			const QString& pPassword)
		{
			const RemoteDeviceDescriptor descr(pDiscovery, pHostAddress);
			QMetaObject::invokeMethod(pConnector.data(),
					"onConnectRequest",
					Qt::QueuedConnection,
					Q_ARG(RemoteDeviceDescriptor, descr),
					Q_ARG(QString, pPassword));
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
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<RemoteDispatcher> >());
				const QSharedPointer<RemoteDispatcher> dispatcher = dispatcherVariant.value<QSharedPointer<RemoteDispatcher> >();
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
				const RemoteErrorCode errorCode = errorCodeVariant.value<RemoteErrorCode>();

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

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// No device name.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const QSharedPointer<const Discovery> msg(new Discovery(QString(), QStringLiteral("0123456789ABCDEF"), 2020, {QStringLiteral("IFDInterface_WebSocket_v0")}));
			sendRequest(connector, hostAddress, msg, QString());
			waitForSignals(&spyError, 1, cSignalTimeoutMs);

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::INVALID_REQUEST}, 2020, QString());
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithNoDeviceInfoIsRejected()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Device information is null.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			sendRequest(connector, hostAddress, QSharedPointer<const Discovery>(), QStringLiteral("secret"));
			waitForSignals(&spyError, 1, cSignalTimeoutMs);

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

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Password is empty.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const QSharedPointer<const Discovery> msg(new Discovery(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), server->getServerPort(), {QStringLiteral("IFDInterface_WebSocket_v0")}));
			sendRequest(connector, hostAddress, msg, QString());
			waitForSignals(&spyError, 1, cSignalTimeoutMs);

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::REMOTE_HOST_REFUSED_CONNECTION}, server->getServerPort(), QStringLiteral("Smartphone1"));
			QCOMPARE(spySuccess.count(), 0);
		}


		void requestWithUnsupportedApiLevelIsRejected()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Currently, only API level 1 is supported.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const QSharedPointer<const Discovery> msg(new Discovery(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), 2020, {QStringLiteral("IFDInterface_WebSocket_v2")}));
			sendRequest(connector, hostAddress, msg, QStringLiteral("secret"));
			waitForSignals(&spyError, 1, cSignalTimeoutMs);

			clientThread.exit();
			QVERIFY(clientThread.wait());

			verifyErrorSignal(spyError, {RemoteErrorCode::NO_SUPPORTED_API_LEVEL}, 2020, QStringLiteral("Smartphone1"));
			QCOMPARE(spySuccess.count(), 0);
		}


		void noServerResponseCausesConnectionErrorOrTimeout()
		{
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spySuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Correct request but no server is running.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const QSharedPointer<const Discovery> msg(new Discovery(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), 2020, {QStringLiteral("IFDInterface_WebSocket_v0")}));
			sendRequest(connector, hostAddress, msg, QString("dummy"));
			waitForSignals(&spyError, 1, cSignalTimeoutMs);

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
			QVERIFY(RemoteHelper::checkAndGenerateKey());
			const KeyPair pair = KeyPair::generate();
			QVERIFY(pair.isValid());

			QSslConfiguration config = SecureStorage::getInstance().getTlsConfigRemote().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setCaCertificates({KeyPair::generate().getCertificate(), settings.getCertificate(), KeyPair::generate().getCertificate()});
			QTest::newRow("paired") << QString() << config << QList<QSslCertificate>({KeyPair::generate().getCertificate(), pair.getCertificate(), KeyPair::generate().getCertificate()});

			config = SecureStorage::getInstance().getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
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

#ifndef QT_NO_NETWORKPROXY
			// Listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
			webSocketServer.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
#endif
			webSocketServer.listen(QHostAddress(QHostAddress::LocalHost));
			const quint16 serverPort = webSocketServer.serverPort();

			// Set up client thread.
			QThread clientThread;

			// Execute test in internal block so that all relevant smart pointers are released before stopping the client thread.
			QSharedPointer<QSignalSpy> remoteDispatcherDestructionSpy;
			{
				const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
				QSignalSpy spyConnectorError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
				QSignalSpy spyConnectorSuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

				connector->moveToThread(&clientThread);
				clientThread.start();

				// Send valid encrypted connect request.
				const QHostAddress hostAddress(QHostAddress::LocalHost);
				const QSharedPointer<const Discovery> msg(new Discovery(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), serverPort, {QStringLiteral("IFDInterface_WebSocket_v0")}));
				sendRequest(connector, hostAddress, msg, psk);

				waitForSignals(&spyConnectorSuccess, 1, cSignalTimeoutMs);
				QCOMPARE(spyConnectorError.count(), 0);
				verifySuccessSignal(spyConnectorSuccess, serverPort);

				const QVariant dispatcherVariant = spyConnectorSuccess.first().at(1);
				QVERIFY(dispatcherVariant.canConvert<QSharedPointer<RemoteDispatcher> >());
				const QSharedPointer<RemoteDispatcher> dispatcher = dispatcherVariant.value<QSharedPointer<RemoteDispatcher> >();
				remoteDispatcherDestructionSpy.reset(new QSignalSpy(dispatcher.data(), &QObject::destroyed));
			}

			QCOMPARE(spySocketError.count(), 0);
			QCOMPARE(spySocketSuccess.count(), 1);

			waitForSignals(remoteDispatcherDestructionSpy.data(), 1, cSignalTimeoutMs);
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

			QSslConfiguration config = SecureStorage::getInstance().getTlsConfigRemote().getConfiguration();
			const KeyPair pair = KeyPair::generate();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			webSocketServer.setSslConfiguration(config);

#ifndef QT_NO_NETWORKPROXY
			// Listening with proxy leads to socket error QNativeSocketEnginePrivate::InvalidProxyTypeString
			webSocketServer.setProxy(QNetworkProxy(QNetworkProxy::NoProxy));
#endif
			webSocketServer.listen(QHostAddress(QHostAddress::LocalHost));
			const quint16 serverPort = webSocketServer.serverPort();

			// Set up client thread.
			QThread clientThread;

			const QSharedPointer<RemoteConnectorImpl> connector(new RemoteConnectorImpl(cConnectTimeoutMs));
			QSignalSpy spyConnectorError(connector.data(), &RemoteConnector::fireRemoteDispatcherError);
			QSignalSpy spyConnectorSuccess(connector.data(), &RemoteConnector::fireRemoteDispatcherCreated);

			connector->moveToThread(&clientThread);
			clientThread.start();

			// Send encrypted connect request with wrong psk.
			const QHostAddress hostAddress(QHostAddress::LocalHost);
			const QSharedPointer<const Discovery> msg(new Discovery(QStringLiteral("Smartphone1"), QStringLiteral("0123456789ABCDEF"), serverPort, {QStringLiteral("IFDInterface_WebSocket_v0")}));
			sendRequest(connector, hostAddress, msg, QStringLiteral("sekret"));

			waitForSignals(&spyConnectorError, 1, cSignalTimeoutMs);
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
