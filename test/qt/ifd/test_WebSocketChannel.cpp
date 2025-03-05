/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketChannel.h"

#include "Env.h"
#include "KeyPair.h"
#include "RemoteServiceSettings.h"
#include "ResourceLoader.h"
#include "SecureStorage.h"
#include "TlsChecker.h"

#include <QWebSocket>
#include <QWebSocketServer>
#include <QtTest>

#include <algorithm>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


class test_WebSocketChannel
	: public QObject
{
	Q_OBJECT

	private:
		const KeyPair mKeyServer = KeyPair::generate();
		const KeyPair mKeyClient = KeyPair::generate();
		QSharedPointer<QWebSocketServer> mServer;
		QSharedPointer<QWebSocket> mClient;
		QSharedPointer<WebSocketChannel> mChannel;

		void setupServerConfig(const QList<QSslCertificate>& pKnownCerts)
		{
			if (mServer)
			{
				QSslConfiguration sslConfig;
				sslConfig.setCiphers(Env::getSingleton<SecureStorage>()->getTlsConfig().getCiphers());
				sslConfig.setLocalCertificate(mKeyServer.getCertificate());
				sslConfig.setPrivateKey(mKeyServer.getKey());
				sslConfig.setCaCertificates(pKnownCerts);
				mServer->setSslConfiguration(sslConfig);
			}
		}


		void setupClientConfig(const QList<QSslCertificate>& pKnownCerts, const QSslKey& pPrivateKey)
		{
			if (mClient)
			{
				QSslConfiguration sslConfig;
				sslConfig.setCiphers(Env::getSingleton<SecureStorage>()->getTlsConfig().getCiphers());
				sslConfig.setLocalCertificateChain(pKnownCerts);
				sslConfig.setPrivateKey(pPrivateKey);
				sslConfig.setCaCertificates({mKeyServer.getCertificate()});
				mClient->setSslConfiguration(sslConfig);
			}
		}


		void establishConnection()
		{
			QVERIFY(mServer->listen(QHostAddress::LocalHost));

			mClient->open(mServer->serverUrl());
			QTRY_VERIFY(mClient->isValid()); // clazy:exclude=qstring-allocations
			QTRY_VERIFY(!mChannel.isNull()); // clazy:exclude=qstring-allocations
		}

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void init()
		{
			mServer.reset(new QWebSocketServer(QStringLiteral("test_WebSocketChannel"), QWebSocketServer::SecureMode));
			connect(mServer.data(), &QWebSocketServer::newConnection, this, [this]{
						QWebSocket* socket = mServer->nextPendingConnection();
						if (socket)
						{
							mChannel.reset(new WebSocketChannel(QSharedPointer<QWebSocket>(socket)));
						}
					});

			mClient.reset(new QWebSocket());
			connect(mClient.data(), &QWebSocket::sslErrors, this, [this](const QList<QSslError>& pErrors) {
						Q_UNUSED(pErrors)
						mClient->ignoreSslErrors();
					});
		}


		void test_send()
		{
			setupServerConfig({mKeyClient.getCertificate()});
			setupClientConfig({mKeyClient.getCertificate()}, mKeyClient.getKey());
			establishConnection();

			QSignalSpy textReceivedSpy(mClient.data(), &QWebSocket::textMessageReceived);

			const auto& text = "Hello, World!"_ba;
			mChannel->send(text);
			QTRY_COMPARE(textReceivedSpy.count(), 1); // clazy:exclude=qstring-allocations
			QCOMPARE(textReceivedSpy.constFirst().constFirst().value<QByteArray>(), text);
		}


		void test_getId_data()
		{
			QTest::addColumn<QList<QSslCertificate>>("certs");
			QTest::addColumn<QSslKey>("privateKey");

			QList<QSslCertificate> certs {mKeyClient.getCertificate()};

			QTest::addRow("Single certificate") << certs << mKeyClient.getKey();

			const auto& keySize = 3000;
			const auto& derivedCert = KeyPair::generate(keySize, mKeyClient.getKey().toPem(), mKeyClient.getCertificate().toPem());
			certs.prepend(derivedCert.getCertificate());
			QTest::addRow("Two certificates (one derived)") << certs << derivedCert.getKey();

			const auto& secondDerivedCert = KeyPair::generate(keySize, derivedCert.getKey().toPem(), derivedCert.getCertificate().toPem());
			certs.prepend(secondDerivedCert.getCertificate());
			QTest::addRow("Three certificates (two derived)") << certs << secondDerivedCert.getKey();

			std::reverse(certs.begin(), certs.end());
			QTest::addRow("Three certificates (two derived) - reversed order") << certs << mKeyClient.getKey();
		}


		void test_getId()
		{
			QFETCH(QList<QSslCertificate>, certs);
			QFETCH(QSslKey, privateKey);

			setupServerConfig(certs);
			setupClientConfig(certs, privateKey);
			establishConnection();

			const auto& selfSignedCert = TlsChecker::getRootCertificate(certs);
			QCOMPARE(mChannel->getId(), RemoteServiceSettings::generateFingerprint(selfSignedCert));
		}


};


QTEST_GUILESS_MAIN(test_WebSocketChannel)
#include "test_WebSocketChannel.moc"
