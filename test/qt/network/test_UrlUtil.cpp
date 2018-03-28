/*!
 * \brief Unit tests for \ref test_UrlUtil
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "TestFileHelper.h"
#include "UrlUtil.h"
#include <QSslCertificate>
#include <QtTest>

using namespace governikus;


class test_UrlUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isMatchingSameOriginPolicy()
		{
			QVERIFY(UrlUtil::isMatchingSameOriginPolicy(QUrl("http://www.web.de/index.html"), QUrl("http://www.web.de:80")));
			QVERIFY(UrlUtil::isMatchingSameOriginPolicy(QUrl("http://www.web.de:8080/index.html"), QUrl("http://www.web.de:8080")));
			QVERIFY(UrlUtil::isMatchingSameOriginPolicy(QUrl("https://www.web.de/index.html"), QUrl("https://www.web.de:443")));
			QVERIFY(UrlUtil::isMatchingSameOriginPolicy(QUrl("https://www.web.de:8443/index.html"), QUrl("https://www.web.de:8443")));

			QVERIFY(!UrlUtil::isMatchingSameOriginPolicy(QUrl("https://www.web.de:8443/index.html"), QUrl("https://www.web.de:8444")));
			QVERIFY(!UrlUtil::isMatchingSameOriginPolicy(QUrl("https://www.web.de:999/index.html"), QUrl("http://www.web.de:999")));
			QVERIFY(!UrlUtil::isMatchingSameOriginPolicy(QUrl("http://www.google.de:999/index.html"), QUrl("http://www.web.de:999")));
		}


};

QTEST_GUILESS_MAIN(test_UrlUtil)
#include "test_UrlUtil.moc"
