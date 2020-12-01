/*!
 * \brief Unit tests for \ref UrlUtil
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "UrlUtil.h"
#include <QSslCertificate>
#include <QtTest>

using namespace governikus;


class test_UrlUtil
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void isMatchingSameOriginPolicy_data()
		{
			QTest::addColumn<QUrl>("url1");
			QTest::addColumn<QUrl>("url2");
			QTest::addColumn<bool>("valid");

			QTest::newRow("none_80") << QUrl("http://www.web.de/index.html") << QUrl("http://www.web.de:80") << true;
			QTest::newRow("8080_8080") << QUrl("http://www.web.de:8080/index.html") << QUrl("http://www.web.de:8080") << true;
			QTest::newRow("none_443") << QUrl("https://www.web.de/index.html") << QUrl("https://www.web.de:443") << true;
			QTest::newRow("8443_8443") << QUrl("https://www.web.de:8443/index.html") << QUrl("https://www.web.de:8443") << true;

			QTest::newRow("false_8443_8444") << QUrl("https://www.web.de:8443/index.html") << QUrl("https://www.web.de:8444") << false;
			QTest::newRow("false_999_999") << QUrl("https://www.web.de:999/index.html") << QUrl("http://www.web.de:999") << false;
			QTest::newRow("false_different_domain") << QUrl("http://www.google.de:999/index.html") << QUrl("http://www.web.de:999") << false;

			QTest::newRow("no_scheme_https_with_port") << QUrl("de.dummy.cz") << QUrl("https://de.dummy.cz:443") << false;
			QTest::newRow("no_scheme_https_without_port") << QUrl("de.dummy.cz") << QUrl("https://de.dummy.cz") << false;

			QTest::newRow("no_scheme_http_with_port") << QUrl("de.dummy.cz") << QUrl("http://de.dummy.cz:80") << false;
			QTest::newRow("no_scheme_http_without_port") << QUrl("de.dummy.cz") << QUrl("http://de.dummy.cz") << false;
		}


		void isMatchingSameOriginPolicy()
		{
			QFETCH(QUrl, url1);
			QFETCH(QUrl, url2);
			QFETCH(bool, valid);

			QCOMPARE(UrlUtil::isMatchingSameOriginPolicy(url1, url2), valid);
		}


		void majorMinor()
		{
			const QString URL_PREFIX("https://www.der-pott-kocht.net:8443/index.html");
			const QUrl url(URL_PREFIX);

			// Ok
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Ok, ECardApiResult::Minor::null))).toString(),
					URL_PREFIX + "?ResultMajor=ok");

			// General server error
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Unknown_Error, QString(), ECardApiResult::Origin::Server))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=serverError");

			// Minors defined in TR-03112-1 and TR-03124-1 2.5.4.2
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Communication_Error))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=communicationError");
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::DP_Trusted_Channel_Establishment_Failed))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=trustedChannelEstablishmentFailed");
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::SAL_Cancellation_by_User))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=cancellationByUser");

			// No difference between client and server origin
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::SAL_Cancellation_by_User, QString(), ECardApiResult::Origin::Server))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=cancellationByUser");

			// General client error
			QCOMPARE(UrlUtil::addMajorMinor(url, GlobalStatus(ECardApiResult(ECardApiResult::Major::Error, ECardApiResult::Minor::AL_Not_Initialized))).toString(),
					URL_PREFIX + "?ResultMajor=error&ResultMinor=clientError");
		}


};

QTEST_GUILESS_MAIN(test_UrlUtil)
#include "test_UrlUtil.moc"
