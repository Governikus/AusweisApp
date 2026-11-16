/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "RemoteTlsServer.h"

#include "AppSettings.h"
#include "KeyPair.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"

#include <QRegularExpression>
#include <QSslCipher>
#include <QSslSocket>
#include <QtTest>

#include <memory>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(SecureStorage::TlsSuite)
Q_DECLARE_METATYPE(KeyPair)

class test_RemoteTlsServer
	: public QObject
{
	Q_OBJECT

	private:
		QByteArray psk;
		const KeyPair pair = KeyPair::generate();

		std::unique_ptr<QSslSocket> createPskClient(const QByteArray& pPsk) const
		{
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pair.getKey());
			config.setLocalCertificate(pair.getCertificate());
			config.setPeerVerifyMode(QSslSocket::VerifyNone);

			auto client = std::make_unique<QSslSocket>();
			client->setSslConfiguration(config);
			connect(client.get(), &QSslSocket::preSharedKeyAuthenticationRequired, client.get(), [pPsk](QSslPreSharedKeyAuthenticator* pAuthenticator){
						pAuthenticator->setPreSharedKey(pPsk);
					});
			return client;
		}


		static bool finishHandshake(QSslSocket& pClient)
		{
			QSignalSpy clientEncrypted(&pClient, &QSslSocket::encrypted);
			QSignalSpy clientErrors(&pClient, &QAbstractSocket::errorOccurred);
			if (pClient.state() == QAbstractSocket::ConnectedState)
			{
				pClient.startClientEncryption();
			}
			const bool finished = QTest::qWaitFor([&clientEncrypted, &clientErrors]{
						return clientEncrypted.count() + clientErrors.count() > 0;
					});
			return finished && clientEncrypted.count() == 1;
		}


		bool connectWithPsk(const RemoteTlsServer& pServer, const QByteArray& pPsk) const
		{
			const auto client = createPskClient(pPsk);
			client->connectToHost(QHostAddress(QHostAddress::LocalHost), pServer.serverPort());
			if (!client->waitForConnected())
			{
				return false;
			}
			return finishHandshake(*client);
		}


		void pairDevices(const KeyPair& pClientPair, const QList<QSslCertificate>& pCertChain, QtMsgType pLogMsgType, const QString& pLogError)
		{
			RemoteTlsServer server;
			QCOMPARE(server.getCurrentCertificate(), QSslCertificate());
			connect(&server, &RemoteTlsServer::firePskChanged, this, [this](const QByteArray& pPsk){
						psk = pPsk;
					});
			server.setPairing();
			server.startListening(0);
			QVERIFY(server.hasPsk());

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd(SecureStorage::TlsSuite::PSK).getConfiguration();
			config.setPrivateKey(pClientPair.getKey());
			config.setLocalCertificateChain(pCertChain);

			bool pskSignalFired = false;
			QSslSocket client;
			client.setSslConfiguration(config);
			connect(&client, QOverload<const QList<QSslError>&>::of(&QSslSocket::sslErrors), this, [&client](const QList<QSslError>& pErrors){
						QList<QSslError> ignoreList;
						for (const auto& error : pErrors)
						{
							if (error.error() == QSslError::SelfSignedCertificate ||
							error.error() == QSslError::SelfSignedCertificateInChain ||
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

			QScopedPointer<QTcpSocket> remoteSocket;
			connect(&server, &RemoteTlsServer::fireNewConnection, this, [&remoteSocket](QTcpSocket* pSocket){
						remoteSocket.reset(pSocket);
					});

			QSignalSpy newConnection(&server, &RemoteTlsServer::fireNewConnection);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);
			QSignalSpy clientErrorOccurred(&client, &QAbstractSocket::errorOccurred);

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());

			if (pLogError.isEmpty())
			{
				QTRY_COMPARE(newConnection.count(), 1); // clazy:exclude=qstring-allocations
				QVERIFY(pskSignalFired);
				QTRY_COMPARE(clientEncrypted.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(clientErrorOccurred.count(), 0);

				QVERIFY(remoteSocket);
				const QByteArray sendData("hello world");
				QSignalSpy spyRead(remoteSocket.data(), &QIODevice::readyRead);
				client.write(sendData);
				QTRY_COMPARE(spyRead.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(remoteSocket->readAll(), sendData);
				QCOMPARE(server.getCurrentCertificate(), pClientPair.getCertificate());
			}
			else
			{
				QTest::ignoreMessage(pLogMsgType, QRegularExpression(pLogError));
				QTRY_COMPARE(clientErrorOccurred.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(client.error(), QAbstractSocket::RemoteHostClosedError);
			}

		}

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
			QCOMPARE(server.hasPsk(), serverPairing);

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
			QCOMPARE(clientErrors.takeFirst().at(0).value<QAbstractSocket::SocketError>(), QAbstractSocket::SocketError::SslHandshakeFailedError);
			QCOMPARE(clientEncrypted.count(), 0);
			QCOMPARE(clientPsk.count(), 0);
		}


		void pairingDisabledAfterFailedAttempts()
		{
			RemoteTlsServer server;
			QByteArray serverPsk;
			connect(&server, &RemoteTlsServer::firePskChanged, this, [&serverPsk](const QByteArray& pPsk){
						serverPsk = pPsk;
					});
			QSignalSpy pairingAborted(&server, &RemoteTlsServer::firePairingAborted);
			server.setPairing();
			QVERIFY(server.startListening(0));
			QVERIFY(server.hasPsk());
			const QByteArray validPsk = serverPsk;
			const QByteArray wrongPsk = validPsk == "0000" ? QByteArray("0001") : QByteArray("0000");

			// connection accepted while pairing is still active, handshake follows later
			std::unique_ptr<QSslSocket> pendingClient = createPskClient(validPsk);
			pendingClient->connectToHost(QHostAddress(QHostAddress::LocalHost), server.serverPort());
			QVERIFY(pendingClient->waitForConnected());
			QTRY_COMPARE(server.mPendingSockets.size(), 1); // clazy:exclude=qstring-allocations

			QVERIFY(!connectWithPsk(server, wrongPsk));
			QTRY_VERIFY(server.hasPsk());
			QVERIFY(!connectWithPsk(server, wrongPsk));
			QTRY_VERIFY(server.hasPsk());
			QCOMPARE(pairingAborted.count(), 0);

			QTest::ignoreMessage(QtWarningMsg, "Too many failed pairing attempts, pairing disabled");
			QVERIFY(!connectWithPsk(server, wrongPsk));
			QTRY_VERIFY(!server.hasPsk());
			QCOMPARE(serverPsk, QByteArray());
			QCOMPARE(pairingAborted.count(), 1);

			// the correct code must not work anymore, not even on a connection accepted before
			QVERIFY(!connectWithPsk(server, validPsk));
			QVERIFY(!finishHandshake(*pendingClient));
			QVERIFY(!server.hasPsk());

			// a new pairing starts with a fresh limit
			server.setPairing();
			QVERIFY(server.hasPsk());
			QVERIFY(!connectWithPsk(server, wrongPsk));
			QTRY_VERIFY(server.hasPsk());
			QCOMPARE(pairingAborted.count(), 1);
		}


		void pskOfConnectionTimeIsUsed()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});

			RemoteTlsServer server;
			QByteArray serverPsk;
			connect(&server, &RemoteTlsServer::firePskChanged, this, [&serverPsk](const QByteArray& pPsk){
						serverPsk = pPsk;
					});
			server.setPairing();
			QVERIFY(server.startListening(0));
			const QByteArray shownPsk = serverPsk;

			std::unique_ptr<QSslSocket> client = createPskClient(shownPsk);
			client->connectToHost(QHostAddress(QHostAddress::LocalHost), server.serverPort());
			QVERIFY(client->waitForConnected());
			QTRY_COMPARE(server.mPendingSockets.size(), 1); // clazy:exclude=qstring-allocations

			// the code is rotated while the handshake is still on its way
			do
			{
				server.setRandomPsk();
			}
			while (serverPsk == shownPsk);

			QVERIFY(finishHandshake(*client));
		}


		void tryReconnectWithoutPairedDeviceAndReconnectedPaired()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			RemoteTlsServer server;
			server.startListening(0);

			QScopedPointer<QTcpSocket> remoteSocket;
			connect(&server, &RemoteTlsServer::fireNewConnection, this, [&remoteSocket](QTcpSocket* pSocket){
						remoteSocket.reset(pSocket);
					});

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
				QVERIFY(!remoteSocket);
			}

			QTRY_VERIFY(server.mSocket.isNull());

			QSslSocket clientPaired;
			config.setCaCertificates(settings.getCertificates());
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
			QTRY_VERIFY(remoteSocket); // clazy:exclude=qstring-allocations
			QTRY_COMPARE(clientPairedEncrypted.count(), 1); // clazy:exclude=qstring-allocations
		}


		void pairDeviceAndReconnect_data()
		{
			QTest::addColumn<KeyPair>("pairedPair");
			QTest::addColumn<QList<QSslCertificate>>("pairedChain");
			QTest::addColumn<QtMsgType>("pairedLogMsgType");
			QTest::addColumn<QString>("pairedLogError");
			QTest::addColumn<KeyPair>("connectPair");
			QTest::addColumn<QList<QSslCertificate>>("connectChain");
			QTest::addColumn<QtMsgType>("connectLogMsgType");
			QTest::addColumn<QString>("connectLogError");

			QTest::newRow("no peer cert pairing") << pair
												  << QList<QSslCertificate>({})
												  << QtDebugMsg
												  << QStringLiteral("The peer did not present any certificate")
												  << pair
												  << QList<QSslCertificate>({})
												  << QtDebugMsg
												  << QStringLiteral("The peer did not present any certificate");

			QTest::newRow("no peer cert reconnect") << pair
													<< QList<QSslCertificate>({pair.getCertificate()})
													<< QtDebugMsg
													<< QString()
													<< pair
													<< QList<QSslCertificate>({})
													<< QtDebugMsg
													<< QStringLiteral("The peer did not present any certificate");

			QTest::newRow("self signed") << pair
										 << QList<QSslCertificate>({pair.getCertificate()})
										 << QtDebugMsg
										 << QString()
										 << pair
										 << QList<QSslCertificate>({pair.getCertificate()})
										 << QtDebugMsg
										 << QString();

			const auto& derived = KeyPair::generate(3072, pair.getKey().toPem(), pair.getCertificate().toPem());
			QTest::newRow("self signed & derived") << pair
												   << QList<QSslCertificate>({pair.getCertificate()})
												   << QtDebugMsg
												   << QString()
												   << derived
												   << QList<QSslCertificate>({derived.getCertificate(), pair.getCertificate()})
												   << QtDebugMsg
												   << QString();

			const auto& derived2 = KeyPair::generate(3072, pair.getKey().toPem(), pair.getCertificate().toPem());
			QTest::newRow("derived") << derived2
									 << QList<QSslCertificate>({derived2.getCertificate(), pair.getCertificate()})
									 << QtDebugMsg
									 << QString()
									 << derived2
									 << QList<QSslCertificate>({derived2.getCertificate(), pair.getCertificate()})
									 << QtDebugMsg
									 << QString();

			QTest::newRow("derived and no root reconnect") << derived2
														   << QList<QSslCertificate>({derived2.getCertificate(), pair.getCertificate()})
														   << QtDebugMsg
														   << QString()
														   << derived2
														   << QList<QSslCertificate>({derived2.getCertificate()})
														   << QtCriticalMsg
														   << QStringLiteral("Client denied... no root certificate found!");
		}


		void pairDeviceAndReconnect()
		{
			QFETCH(KeyPair, pairedPair);
			QFETCH(QList<QSslCertificate>, pairedChain);
			QFETCH(QtMsgType, pairedLogMsgType);
			QFETCH(QString, pairedLogError);
			QFETCH(KeyPair, connectPair);
			QFETCH(QList<QSslCertificate>, connectChain);
			QFETCH(QtMsgType, connectLogMsgType);
			QFETCH(QString, connectLogError);

			auto& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
			settings.setTrustedCertificates({});
			pairDevices(pairedPair, pairedChain, pairedLogMsgType, pairedLogError);
			QVERIFY(!QTest::currentTestFailed());

			RemoteTlsServer server;
			server.startListening(0);
			QVERIFY(!settings.getCertificates().isEmpty());

			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigRemoteIfd().getConfiguration();
			config.setPrivateKey(connectPair.getKey());
			config.setLocalCertificateChain(connectChain);
			config.setCaCertificates(settings.getCertificates());

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
			QSignalSpy clientErrorOccurred(&client, &QAbstractSocket::errorOccurred);

			QScopedPointer<QTcpSocket> remoteSocket;
			connect(&server, &RemoteTlsServer::fireNewConnection, this, [&remoteSocket](QTcpSocket* pSocket){
						remoteSocket.reset(pSocket);
					});

			client.connectToHostEncrypted(QHostAddress(QHostAddress::LocalHost).toString(), server.serverPort());
			if (connectLogError.isEmpty())
			{
				QTRY_VERIFY(remoteSocket); // clazy:exclude=qstring-allocations
				QTRY_COMPARE(clientEncrypted.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(clientErrorOccurred.count(), 0);
			}
			else
			{
				QTest::ignoreMessage(connectLogMsgType, QRegularExpression(connectLogError));
				QTRY_COMPARE(clientErrorOccurred.count(), 1); // clazy:exclude=qstring-allocations
				QCOMPARE(client.error(), QAbstractSocket::RemoteHostClosedError);
			}
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
