/*!
 * \brief Unit tests for \ref RemoteTlsServer
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteTlsServer.h"

#include "AppSettings.h"
#include "KeyPair.h"
#include "SecureStorage.h"

#include <QSslCipher>
#include <QSslSocket>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(SecureStorage::TlsSuite)

class test_RemoteTlsServer
	: public QObject
{
	Q_OBJECT

	private:
		QByteArray psk;
		const KeyPair pair = KeyPair::generate();

	private Q_SLOTS:
		void checkFailingConnectionOnDifferentMode_data()
		{
			QTest::addColumn<bool>("serverPairing");
			QTest::addColumn<SecureStorage::TlsSuite>("clientConfig");

			QTest::newRow("server in pairing mode") << true << SecureStorage::TlsSuite::DEFAULT;
			QTest::newRow("client in pairing mode") << false << SecureStorage::TlsSuite::PSK;
		}


		void checkFailingConnectionOnDifferentMode()
		{
			QFETCH(bool, serverPairing);
			QFETCH(SecureStorage::TlsSuite, clientConfig);

			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({pair.getCertificate()});
			RemoteTlsServer server;
			server.setPairing(serverPairing);
			server.listen();

			auto config = SecureStorage::getInstance().getTlsConfigRemote(clientConfig).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			QSslSocket client;
			client.setSslConfiguration(config);

			qRegisterMetaType<QSslPreSharedKeyAuthenticator>("QSslPreSharedKeyAuthenticator*");
			QSignalSpy clientPsk(&client, &QSslSocket::preSharedKeyAuthenticationRequired);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);
			QSignalSpy clientErrors(&client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error));
			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			QTRY_COMPARE(clientErrors.count(), 1);
			QCOMPARE(clientErrors.takeFirst().at(0).value<QAbstractSocket::SocketError>(), QAbstractSocket::SocketError::RemoteHostClosedError);
			QCOMPARE(clientEncrypted.count(), 0);
			QCOMPARE(clientPsk.count(), 0);
		}


		void requiredPskForPairing()
		{
			RemoteTlsServer server;
			QCOMPARE(server.getCurrentCertificate(), QSslCertificate());
			connect(&server, &RemoteTlsServer::firePskChanged, this, [this](const QByteArray& pPsk){
						psk = pPsk;
					});
			server.setPairing();
			server.listen();

			auto config = SecureStorage::getInstance().getTlsConfigRemote(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			bool pskSignalFired = false;
			QSslSocket client;
			client.setSslConfiguration(config);
			connect(&client, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&](const QList<QSslError>& pErrors){
						QList<QSslError> ignoreList;
						for (const auto& error : pErrors)
						{
							if (error.error() == QSslError::SelfSignedCertificate ||
							error.error() == QSslError::HostNameMismatch)
							{
								ignoreList << error;
							}
						}
						client.ignoreSslErrors(ignoreList);
					});

			connect(&client, &QSslSocket::preSharedKeyAuthenticationRequired, this, [&](QSslPreSharedKeyAuthenticator* pAuthenticator){
						pAuthenticator->setPreSharedKey(psk);
						pskSignalFired = true;
					});

			QTcpSocket* remoteSocket = nullptr;
			connect(&server, &RemoteTlsServer::newConnection, this, [&](QTcpSocket* pSocket){
						remoteSocket = pSocket;
					});

			QSignalSpy newConnection(&server, &RemoteTlsServer::newConnection);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			QTRY_COMPARE(newConnection.count(), 1);
			QVERIFY(pskSignalFired);
			QTRY_COMPARE(clientEncrypted.count(), 1);

			QVERIFY(remoteSocket);
			const QByteArray sendData("hello world");
			QSignalSpy spyRead(remoteSocket, &QIODevice::readyRead);
			client.write(sendData);
			QTRY_COMPARE(spyRead.count(), 1);
			QCOMPARE(remoteSocket->readAll(), sendData);
			QCOMPARE(server.getCurrentCertificate(), pair.getCertificate());
		}


		void tryReconnectWithoutPairedDeviceAndReconnectedPaired()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			RemoteTlsServer server;
			server.listen();
			QSignalSpy newConnection(&server, &RemoteTlsServer::newConnection);

			auto config = SecureStorage::getInstance().getTlsConfigRemote().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			{
				QSslSocket client;
				client.setSslConfiguration(config);
				client.ignoreSslErrors({QSslError::SelfSignedCertificate, QSslError::HostNameMismatch});

				QSignalSpy clientFailed(&client, &QAbstractSocket::disconnected);
				client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
				QTRY_COMPARE(clientFailed.count(), 1);
				QCOMPARE(newConnection.count(), 0);
			}

			QSslSocket clientPaired;
			config.setCaCertificates({settings.getCertificate()});
			clientPaired.setSslConfiguration(config);
			connect(&clientPaired, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&](const QList<QSslError>& pErrors){
						QList<QSslError> ignoreList;
						for (const auto& error : pErrors)
						{
							if (error.error() == QSslError::HostNameMismatch)
							{
								ignoreList << error;
							}
						}
						clientPaired.ignoreSslErrors(ignoreList);
					});
			settings.addTrustedCertificate(config.localCertificate());

			QSignalSpy clientPairedEncrypted(&clientPaired, &QSslSocket::encrypted);
			clientPaired.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
			QTRY_COMPARE(newConnection.count(), 1);
			QTRY_COMPARE(clientPairedEncrypted.count(), 1);
		}


		void pairDeviceAndReconnect()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			requiredPskForPairing();
			QVERIFY(settings.checkAndGenerateKey());

			RemoteTlsServer server;
			server.listen();

			auto config = SecureStorage::getInstance().getTlsConfigRemote().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setCaCertificates({settings.getCertificate()});

			QSslSocket client;
			client.setSslConfiguration(config);
			connect(&client, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&](const QList<QSslError>& pErrors){
						QList<QSslError> ignoreList;
						for (const auto& error : pErrors)
						{
							if (error.error() == QSslError::HostNameMismatch)
							{
								ignoreList << error;
							}
						}
						client.ignoreSslErrors(ignoreList);
					});

			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);
			QSignalSpy newConnection(&server, &RemoteTlsServer::newConnection);

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
			QTRY_COMPARE(newConnection.count(), 1);
			QTRY_COMPARE(clientEncrypted.count(), 1);
		}


		void setPairing()
		{
			RemoteTlsServer server;
			QSignalSpy pskChanged(&server, &RemoteTlsServer::firePskChanged);

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 0);

			server.setPairing();
			QTRY_COMPARE(pskChanged.count(), 1);

			server.setPairing();
			QTRY_COMPARE(pskChanged.count(), 2);

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 3);

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 3);
		}


		void checkPskSize()
		{
			RemoteTlsServer server;
			QSignalSpy pskChanged(&server, &RemoteTlsServer::firePskChanged);
			for (int i = 0; i < 100; ++i)
			{
				server.setPairing();
				QTRY_COMPARE(pskChanged.count(), 1);
				const auto& pin = pskChanged.takeFirst().at(0).toByteArray();
				QCOMPARE(pin.size(), 4);
			}
		}


};


QTEST_GUILESS_MAIN(test_RemoteTlsServer)
#include "test_RemoteTlsServer.moc"
