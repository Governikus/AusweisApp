/*!
 * \brief Unit tests for \ref RemoteTlsServer
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteTlsServer.h"

#include "AppSettings.h"
#include "KeyPair.h"
#include "ResourceLoader.h"
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
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


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
			server.startListening(0);

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(clientConfig).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			QSslSocket client;
			client.setSslConfiguration(config);

			qRegisterMetaType<QSslPreSharedKeyAuthenticator>("QSslPreSharedKeyAuthenticator*");
			QSignalSpy clientPsk(&client, &QSslSocket::preSharedKeyAuthenticationRequired);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);
			QSignalSpy clientErrors(&client, &QAbstractSocket::errorOccurred);
			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			QTRY_COMPARE(clientErrors.count(), 1); // clazy:exclude=qstring-allocations
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
			QCOMPARE(clientErrors.takeFirst().at(0).value<QAbstractSocket::SocketError>(), QAbstractSocket::SocketError::SslHandshakeFailedError);
#else
			QCOMPARE(clientErrors.takeFirst().at(0).value<QAbstractSocket::SocketError>(), QAbstractSocket::SocketError::RemoteHostClosedError);
#endif
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
			server.startListening(0);

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			bool pskSignalFired = false;
			QSslSocket client;
			client.setSslConfiguration(config);
			connect(&client, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&client](const QList<QSslError>& pErrors){
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

			connect(&client, &QSslSocket::preSharedKeyAuthenticationRequired, this, [&pskSignalFired, this](QSslPreSharedKeyAuthenticator* pAuthenticator){
					pAuthenticator->setPreSharedKey(psk);
					pskSignalFired = true;
				});

			QTcpSocket* remoteSocket = nullptr;
			connect(&server, &RemoteTlsServer::fireNewConnection, this, [&remoteSocket](QTcpSocket* pSocket){
					remoteSocket = pSocket;
				});

			QSignalSpy newConnection(&server, &RemoteTlsServer::fireNewConnection);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			QTRY_COMPARE(newConnection.count(), 1); // clazy:exclude=qstring-allocations
			QVERIFY(pskSignalFired);
			QTRY_COMPARE(clientEncrypted.count(), 1); // clazy:exclude=qstring-allocations

			QVERIFY(remoteSocket);
			const QByteArray sendData("hello world");
			QSignalSpy spyRead(remoteSocket, &QIODevice::readyRead);
			client.write(sendData);
			QTRY_COMPARE(spyRead.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(remoteSocket->readAll(), sendData);
			QCOMPARE(server.getCurrentCertificate(), pair.getCertificate());
		}


		void tryReconnectWithoutPairedDeviceAndReconnectedPaired()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			RemoteTlsServer server;
			server.startListening(0);
			QSignalSpy newConnection(&server, &RemoteTlsServer::fireNewConnection);

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());

			{
				QSslSocket client;
				client.setSslConfiguration(config);
				const QList<QSslError> errors({QSslError(QSslError::SelfSignedCertificate), QSslError(QSslError::HostNameMismatch)});
				client.ignoreSslErrors(errors);

				QSignalSpy clientFailed(&client, &QAbstractSocket::disconnected);
				client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
				QTRY_COMPARE(clientFailed.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(newConnection.count(), 0);
			}

			QSslSocket clientPaired;
			config.setCaCertificates({settings.getCertificate()});
			clientPaired.setSslConfiguration(config);
			connect(&clientPaired, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&clientPaired](const QList<QSslError>& pErrors){
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
			QTRY_COMPARE(newConnection.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(clientPairedEncrypted.count(), 1); // clazy:exclude=qstring-allocations
		}


		void pairDeviceAndReconnect()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			requiredPskForPairing();
			QVERIFY(settings.checkAndGenerateKey());

			RemoteTlsServer server;
			server.startListening(0);

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setCaCertificates({settings.getCertificate()});

			QSslSocket client;
			client.setSslConfiguration(config);
			connect(&client, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&client](const QList<QSslError>& pErrors){
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
			QSignalSpy newConnection(&server, &RemoteTlsServer::fireNewConnection);

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
			QTRY_COMPARE(newConnection.count(), 1); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(clientEncrypted.count(), 1); // clazy:exclude=qstring-allocations
		}


		void setPairing()
		{
			RemoteTlsServer server;
			QSignalSpy pskChanged(&server, &RemoteTlsServer::firePskChanged);

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 0); // clazy:exclude=qstring-allocations

			server.setPairing();
			QTRY_COMPARE(pskChanged.count(), 1); // clazy:exclude=qstring-allocations

			server.setPairing();
			QTRY_COMPARE(pskChanged.count(), 2); // clazy:exclude=qstring-allocations

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 3); // clazy:exclude=qstring-allocations

			server.setPairing(false);
			QTRY_COMPARE(pskChanged.count(), 3); // clazy:exclude=qstring-allocations
		}


		void checkPskSize()
		{
			bool flipEnabled = true;
			RemoteTlsServer server;
			QSignalSpy pskChanged(&server, &RemoteTlsServer::firePskChanged);
			for (int i = 0; i < 100; ++i)
			{
				server.setPairing(flipEnabled);
				QTRY_COMPARE(pskChanged.count(), 1); // clazy:exclude=qstring-allocations
				const auto& pin = pskChanged.takeFirst().at(0).toByteArray();
				QCOMPARE(pin.size(), flipEnabled ? 4 : 0);
				flipEnabled = !flipEnabled;
			}
		}


};


QTEST_GUILESS_MAIN(test_RemoteTlsServer)
#include "test_RemoteTlsServer.moc"
