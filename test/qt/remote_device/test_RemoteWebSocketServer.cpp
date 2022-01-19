/*!
 * \brief Unit tests for \ref RemoteWebSocketServer
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteWebSocketServer.h"

#include "AppSettings.h"
#include "KeyPair.h"
#include "NetworkManager.h"
#include "SecureStorage.h"

#include <QByteArray>
#include <QtTest>
#include <QWebSocket>

using namespace governikus;


class PskHandler
	: public QObject
{
	Q_OBJECT

	public:
		QByteArray mPsk;
		QWebSocket* mWebSocket;

		PskHandler(QWebSocket* pWebSocket, RemoteWebSocketServer* pServer = nullptr)
			: QObject()
			, mPsk()
			, mWebSocket(pWebSocket)
		{
			connect(pWebSocket, &QWebSocket::preSharedKeyAuthenticationRequired, this, &PskHandler::onPreSharedKeyAuthenticationRequired);
			connect(pWebSocket, &QWebSocket::sslErrors, this, &PskHandler::onSslErrors);

			if (pServer)
			{
				connect(pServer, &RemoteWebSocketServer::firePskChanged, this, [this](const QByteArray& pPsk){
						mPsk = pPsk;
					});
			}
		}

	private Q_SLOTS:
		void onPreSharedKeyAuthenticationRequired(QSslPreSharedKeyAuthenticator* pAuthenticator)
		{
			qDebug() << "PSK requested on CLIENT:" << mPsk;
			pAuthenticator->setPreSharedKey(mPsk);
		}


		void onSslErrors(const QList<QSslError>& pErrors)
		{
			QList<QSslError::SslError> allowedErrors = {
				QSslError::HostNameMismatch
			};

			const auto& config = mWebSocket->sslConfiguration();
			const auto& pairingCiphers = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getCiphers();
			if (pairingCiphers.contains(config.sessionCipher()))
			{
				allowedErrors << QSslError::SelfSignedCertificate;
			}

			bool ignoreErrors = true;
			for (const auto& error : pErrors)
			{
				if (!allowedErrors.contains(error.error()))
				{
					ignoreErrors = false;
					break;
				}
			}

			if (ignoreErrors)
			{
				qDebug() << "Ignore TLS error:" << pErrors;
				mWebSocket->ignoreSslErrors();
				return;
			}

			qDebug() << "TLS error found:" << pErrors;
		}


};


class test_RemoteWebSocketServer
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<RemoteWebSocketServer> mServer;

	private Q_SLOTS:
		void initTestCase()
		{
			NetworkManager::setApplicationProxyFactory();
		}


		void init()
		{
			mServer.reset(new RemoteWebSocketServerImpl());
		}


		void cleanup()
		{
			mServer.reset();
		}


		void isListening()
		{
			QVERIFY(!mServer->isListening());

			mServer->listen(QStringLiteral("TestServer"));
			QVERIFY(mServer->isListening());

			mServer->close();
			QVERIFY(!mServer->isListening());

			mServer->listen(QStringLiteral("TestServer"));
			QVERIFY(mServer->isListening());
		}


		void listen()
		{
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));
		}


		void listenTwice()
		{
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));
			QVERIFY(!mServer->listen(QStringLiteral("TestServer")));
		}


		void getServerPort()
		{
			QVERIFY(mServer->getServerPort() == 0);
			mServer->listen(QStringLiteral("TestServer"));
			QVERIFY(mServer->getServerPort() != 0);
		}


		void getServerName()
		{
			mServer->listen(QStringLiteral("TestServer"));
			QCOMPARE(mServer->getServerName(), QStringLiteral("TestServer"));
		}


		void pairingToListeningServer()
		{
			QCOMPARE(mServer->getCurrentCertificate(), QSslCertificate());
			KeyPair pair = KeyPair::generate();
			QWebSocket client;
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			client.setSslConfiguration(config);
			QSignalSpy spy(&client, &QWebSocket::connected);

			PskHandler pskHandler(&client, mServer.data());
			mServer->setPairing();
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));

			client.open(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(client.state(), QAbstractSocket::SocketState::ConnectedState);
			QCOMPARE(mServer->getCurrentCertificate(), client.sslConfiguration().localCertificate());
		}


		void reconnectToListeningServer()
		{
			QCOMPARE(mServer->getCurrentCertificate(), QSslCertificate());
			KeyPair pair = KeyPair::generate();
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({pair.getCertificate()});
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));

			QWebSocket client;
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setCaCertificates({settings.getCertificate()});
			client.setSslConfiguration(config);
			QSignalSpy spy(&client, &QWebSocket::connected);

			PskHandler pskHandler(&client);
			client.open(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(client.state(), QAbstractSocket::SocketState::ConnectedState);
			QCOMPARE(mServer->getCurrentCertificate(), client.sslConfiguration().localCertificate());
		}


		void connectToClosedServer()
		{
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));

			QUrl serverUrl(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));
			mServer->close();

			QWebSocket client;

			QSignalSpy spy(&client, &QWebSocket::connected);
			client.open(serverUrl);

			spy.wait(500);
			QCOMPARE(spy.count(), 0);
			QVERIFY(client.state() != QAbstractSocket::SocketState::ConnectedState);
		}


		void onlyOneConnection()
		{
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));

			KeyPair pair = KeyPair::generate();
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			QWebSocket client1, client2;
			client1.setSslConfiguration(config);
			client2.setSslConfiguration(config);

			PskHandler pskHandler1(&client1, mServer.data());
			PskHandler pskHandler2(&client2, mServer.data());
			mServer->setPairing();

			QSignalSpy spy1(&client1, &QWebSocket::connected);
			QSignalSpy spy2(&client2, &QWebSocket::disconnected);

			client1.open(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));
			QTRY_COMPARE(spy1.count(), 1); // clazy:exclude=qstring-allocations

			client2.open(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));
			QTRY_COMPARE(spy2.count(), 1); // clazy:exclude=qstring-allocations

			QCOMPARE(client1.state(), QAbstractSocket::SocketState::ConnectedState);
			QCOMPARE(client2.state(), QAbstractSocket::SocketState::UnconnectedState);
		}


		void isConnected()
		{
#if defined(Q_OS_FREEBSD)
			QSKIP("Test not supported on this platform");
#endif

			QVERIFY(!mServer->isConnected());
			QVERIFY(mServer->listen(QStringLiteral("TestServer")));
			QVERIFY(!mServer->isConnected());

			QSignalSpy serverConnectedSpy(mServer.data(), &RemoteWebSocketServer::fireConnectedChanged);

			KeyPair pair = KeyPair::generate();
			QWebSocket client;
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			client.setSslConfiguration(config);

			PskHandler pskHandler(&client, mServer.data());
			mServer->setPairing();
			client.open(QString("wss://127.0.0.1:").append(QString::number(mServer->getServerPort())));

			QTRY_COMPARE(serverConnectedSpy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(serverConnectedSpy[0][0].toBool());
			QVERIFY(mServer->isConnected());

			QSignalSpy serverDisconnectedSpy(mServer.data(), &RemoteWebSocketServer::fireConnectedChanged);
			client.close();

			QTRY_COMPARE(serverDisconnectedSpy.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(!serverDisconnectedSpy[0][0].toBool());
			QVERIFY(!mServer->isConnected());
		}


};


QTEST_GUILESS_MAIN(test_RemoteWebSocketServer)
#include "test_RemoteWebSocketServer.moc"
