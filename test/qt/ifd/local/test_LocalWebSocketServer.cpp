/*!
 * \brief Unit tests for \ref LocalWebSocketServer
 *
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LocalWebSocketServer.h"

#include "Env.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"

#include <QHostAddress>
#include <QWebSocket>
#include <QtTest>

using namespace governikus;

class test_LocalWebSocketServer
	: public QObject
{
	Q_OBJECT

	public:
		const QString mPsk = QStringLiteral("deadbeef");
		LocalWebSocketServer* mServer;

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
			mServer = Env::create<LocalWebSocketServer*>();
			mServer->setPsk(mPsk.toUtf8());
			// pPort = 0 uses a random port to avoid conflicts when running several unittests simultaneously
			QVERIFY(mServer->listen(QStringLiteral("test_LocalWebSocketServer"), 0));
		}


		void connectWithPsk()
		{
			QVERIFY(mServer->isListening());

			QWebSocket client;
			auto config = Env::getSingleton<SecureStorage>()->getTlsConfigLocalIfd().getConfiguration();
			config.setPeerVerifyMode(QSslSocket::VerifyNone);
			client.setSslConfiguration(config);

			connect(&client, &QWebSocket::preSharedKeyAuthenticationRequired, this, [this](QSslPreSharedKeyAuthenticator* pAuthenticator)
				{
					pAuthenticator->setPreSharedKey(mPsk.toUtf8());
				});

			QSignalSpy connectedSpy(&client, &QWebSocket::connected);

			QUrl host;
			host.setScheme(QStringLiteral("wss"));
			host.setHost(mServer->getServerAddress().toString());
			host.setPort(mServer->getServerPort());
			client.open(host);
			QTRY_COMPARE(connectedSpy.count(), 1);
			QCOMPARE(client.state(), QAbstractSocket::SocketState::ConnectedState);
		}


};

QTEST_GUILESS_MAIN(test_LocalWebSocketServer)
#include "test_LocalWebSocketServer.moc"
