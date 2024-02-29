/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ProxyCredentials
 */

#include "ProxyCredentials.h"

#include <QElapsedTimer>
#include <QTimer>
#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;


Q_DECLARE_METATYPE(QNetworkProxy::ProxyType)


class test_ProxyCredentials
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_Url_data()
		{
			QTest::addColumn<QNetworkProxy::ProxyType>("type");
			QTest::addColumn<QString>("url");

			QTest::newRow("NoProxy") << QNetworkProxy::NoProxy << "http://proxy.example.com:1337";
			QTest::newRow("DefaultProxy") << QNetworkProxy::DefaultProxy << "http://proxy.example.com:1337";
			QTest::newRow("Socks5Proxy") << QNetworkProxy::Socks5Proxy << "socks5://proxy.example.com:1337";
			QTest::newRow("HttpProxy") << QNetworkProxy::HttpProxy << "http://proxy.example.com:1337";
			QTest::newRow("HttpCachingProxy") << QNetworkProxy::HttpCachingProxy << "http://proxy.example.com:1337";
			QTest::newRow("FtpCachingProxy") << QNetworkProxy::FtpCachingProxy << "ftp://proxy.example.com:1337";
		}


		void test_Url()
		{
			QFETCH(QNetworkProxy::ProxyType, type);
			QFETCH(QString, url);

			QNetworkProxy proxy(QNetworkProxy::HttpProxy, "proxy.example.com"_L1, 1337, "Test"_L1);
			proxy.setType(type);
			QAuthenticator authenticator;

			ProxyCredentials proxyCredentials(proxy, &authenticator);
			QCOMPARE(proxyCredentials.getUrl(), url);
		}


		void test_Credentials()
		{
			QNetworkProxy proxy(QNetworkProxy::HttpProxy, "proxy.example.com"_L1, 1337, "Test"_L1);
			QAuthenticator authenticator;

			ProxyCredentials proxyCredentials(proxy, &authenticator);
			QCOMPARE(proxyCredentials.getProposedUser(), "Test"_L1);

			QCOMPARE(proxyCredentials.getUser(), ""_L1);
			proxyCredentials.setUser("TestUser"_L1);
			QCOMPARE(proxyCredentials.getUser(), "TestUser"_L1);

			QCOMPARE(proxyCredentials.getPassword(), ""_L1);
			proxyCredentials.setPassword("TestPassword"_L1);
			QCOMPARE(proxyCredentials.getPassword(), "TestPassword"_L1);
		}


		void test_BlockingWait()
		{
			const int waitFor = 500;
			const int tolerance = 10;

			QNetworkProxy proxy(QNetworkProxy::HttpProxy, "proxy.example.com"_L1, 1337, "Test"_L1);
			QAuthenticator authenticator;
			ProxyCredentials proxyCredentials(proxy, &authenticator);

			QElapsedTimer timer;
			timer.start();

			QTimer::singleShot(waitFor, this, [&proxyCredentials] {
					proxyCredentials.confirmInput();
				});

			proxyCredentials.waitForConfirmation();
			QVERIFY(timer.elapsed() >= waitFor - tolerance);
		}


};

QTEST_GUILESS_MAIN(test_ProxyCredentials)
#include "test_ProxyCredentials.moc"
