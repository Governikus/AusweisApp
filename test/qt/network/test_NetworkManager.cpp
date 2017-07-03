/*!
 * \brief Unit tests for \ref NetworkManager
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "AppSettings.h"
#include "LogHandler.h"
#include "NetworkManager.h"

#include <QtCore>
#include <QtNetwork>
#include <QtTest>

using namespace governikus;


class test_NetworkManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			AppSettings::getInstance().load();
			LogHandler::getInstance().init();
		}


		void cleanup()
		{
			LogHandler::getInstance().resetBacklog();
		}


		void paosRequestAttached()
		{
			QNetworkRequest request(QUrl("https://dummy"));
			NetworkManager manager;
			auto reply = manager.paos(request, "content", false, 1);
			QCOMPARE(reply->request(), request);
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 6);
			QVERIFY(request.sslConfiguration().ellipticCurves().contains(QSslEllipticCurve::fromLongName("prime256v1")));
			QCOMPARE(request.sslConfiguration().ciphers().size(), 24);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("ECDHE-RSA-AES256-GCM-SHA384")));
		}


		void paosRequestPsk()
		{
			QNetworkRequest request(QUrl("https://dummy"));
			NetworkManager manager;
			auto reply = manager.paos(request, "content", true, 1);
			QCOMPARE(reply->request(), request);
			QCOMPARE(request.sslConfiguration().ellipticCurves().size(), 0);
			QCOMPARE(request.sslConfiguration().ciphers().size(), 5);
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-CBC-SHA")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-CBC-SHA256")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES128-GCM-SHA256")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-CBC-SHA384")));
			QVERIFY(request.sslConfiguration().ciphers().contains(QSslCipher("RSA-PSK-AES256-GCM-SHA384")));
		}


};

QTEST_GUILESS_MAIN(test_NetworkManager)
#include "test_NetworkManager.moc"
