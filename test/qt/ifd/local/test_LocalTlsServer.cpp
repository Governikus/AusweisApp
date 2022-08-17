/*!
 * \brief Unit tests for \ref LocalTlsServer
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LocalTlsServer.h"

#include "Env.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"

#include <QSslSocket>
#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(QHostAddress::SpecialAddress)

class test_LocalTlsServer
	: public QObject
{
	Q_OBJECT

	public:
		const QString mPsk = QStringLiteral("deadbeef");
		LocalTlsServer mServer;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			mServer.setPsk(mPsk.toUtf8());
			// pPort = 0 uses a random port to avoid conflicts when running several unittests simultaneously
			QVERIFY(mServer.startListening(0));
		}


		void listenAddress()
		{
			const QVector<QHostAddress> allowedHosts = {QHostAddress::LocalHostIPv6, QHostAddress::LocalHost};
			QVERIFY(allowedHosts.contains(mServer.serverAddress()));
		}


		void connectWithPsk()
		{
			QVERIFY(mServer.isListening());

			QSslSocket client;
			QSslConfiguration config = Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
			config.setPeerVerifyMode(QSslSocket::VerifyNone);
			client.setSslConfiguration(config);
			connect(&client, &QSslSocket::preSharedKeyAuthenticationRequired, this, [this](QSslPreSharedKeyAuthenticator* pAuthenticator)
				{
					pAuthenticator->setPreSharedKey(mPsk.toUtf8());
				});

			QTcpSocket* remoteSocket = nullptr;
			connect(&mServer, &LocalTlsServer::fireNewConnection, this, [&remoteSocket](QTcpSocket* pSocket)
				{
					remoteSocket = pSocket;
				});
			QSignalSpy newConnection(&mServer, &LocalTlsServer::fireNewConnection);
			QSignalSpy clientEncrypted(&client, &QSslSocket::encrypted);

			client.connectToHostEncrypted(mServer.serverAddress().toString(), mServer.serverPort());

			QTRY_COMPARE(newConnection.count(), 1);
			QTRY_COMPARE(clientEncrypted.count(), 1);
			QVERIFY(remoteSocket);

			const QByteArray sendData("hello world");
			QSignalSpy spyRead(remoteSocket, &QIODevice::readyRead);
			client.write(sendData);
			QTRY_COMPARE(spyRead.count(), 1);
			QCOMPARE(remoteSocket->readAll(), sendData);
		}


};

QTEST_GUILESS_MAIN(test_LocalTlsServer)
#include "test_LocalTlsServer.moc"
