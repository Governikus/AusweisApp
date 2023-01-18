/*!
 * \brief Unit tests for \ref UrlUtil
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UrlUtil.h"

#include "AppSettings.h"
#include "Env.h"

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


		void getRequest_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<UrlQueryRequest>("type");
			QTest::addColumn<QString>("value");

			QTest::newRow("empty") << QUrl("") << UrlQueryRequest::UNKNOWN << QString();
			QTest::newRow("no query") << QUrl("example.com") << UrlQueryRequest::UNKNOWN << QString();
			QTest::newRow("empty value") << QUrl("?showui=") << UrlQueryRequest::SHOWUI << QString();
			QTest::newRow("showui - l") << QUrl("?showui=test") << UrlQueryRequest::SHOWUI << QString("test");
			QTest::newRow("showui - h") << QUrl("?SHOWUI=test") << UrlQueryRequest::SHOWUI << QString("test");
			QTest::newRow("status - l") << QUrl("?status=test") << UrlQueryRequest::STATUS << QString("test");
			QTest::newRow("status - h") << QUrl("?STATUS=test") << UrlQueryRequest::STATUS << QString("test");
			QTest::newRow("tctokenurl - l") << QUrl("?tctokenurl=test") << UrlQueryRequest::TCTOKENURL << QString("test");
			QTest::newRow("tctokenurl - h") << QUrl("?TCTOKENURL=test") << UrlQueryRequest::TCTOKENURL << QString("test");
			QTest::newRow("multi 1") << QUrl("?showui=test1&status=test2") << UrlQueryRequest::SHOWUI << QString("test1");
			QTest::newRow("multi 2") << QUrl("?status=test1&showui=test2") << UrlQueryRequest::STATUS << QString("test1");
			QTest::newRow("multi 3") << QUrl("?foo=test1&showui=test2") << UrlQueryRequest::SHOWUI << QString("test2");
			QTest::newRow("multi 4") << QUrl("?foo=&showui=test2") << UrlQueryRequest::SHOWUI << QString("test2");
			QTest::newRow("multi 5") << QUrl("?foo=test1&showui=") << UrlQueryRequest::SHOWUI << QString();
			QTest::newRow("multi 6") << QUrl("?showui=&showui=test1") << UrlQueryRequest::SHOWUI << QString();
			QTest::newRow("multi 7") << QUrl("?foo=test1&bar=test2") << UrlQueryRequest::UNKNOWN << QString();
		}


		void getRequest()
		{
			QFETCH(QUrl, url);
			QFETCH(UrlQueryRequest, type);
			QFETCH(QString, value);

			const auto queryUrl = QUrlQuery(url);
			const auto [parsedType, parsedValue] = UrlUtil::getRequest(queryUrl);
			QCOMPARE(type, parsedType);
			QCOMPARE(value, parsedValue);
		}


		void setHiddenSettings_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<bool>("useTestUri");
			QTest::addColumn<bool>("enableSimulator");

			QTest::newRow("empty") << QUrl("") << false << false;
			QTest::newRow("useTestUri") << QUrl("?useTestUri=true") << true << false;
			QTest::newRow("!useTestUri") << QUrl("?useTestUri=false") << false << false;
			QTest::newRow("enableSimulator") << QUrl("?enableSimulator=true") << false << true;
			QTest::newRow("!enableSimulator") << QUrl("?enableSimulator=false") << false << false;
			QTest::newRow("multi 1") << QUrl("?useTestUri=true&enableSimulator=true") << true << true;
			QTest::newRow("multi 2") << QUrl("?useTestUri=true&enableSimulator=false") << true << false;
		}


		void setHiddenSettings()
		{
			auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			generalSettings.setDeveloperOptions(true);
			generalSettings.setUseSelfauthenticationTestUri(false);
			generalSettings.setSimulatorEnabled(false);

			QCOMPARE(generalSettings.useSelfAuthTestUri(), false);
			QCOMPARE(generalSettings.isSimulatorEnabled(), false);

			QFETCH(QUrl, url);
			QFETCH(bool, useTestUri);
			QFETCH(bool, enableSimulator);

			UrlUtil::setHiddenSettings(QUrlQuery(url));

			QCOMPARE(generalSettings.useSelfAuthTestUri(), useTestUri);
			QCOMPARE(generalSettings.isSimulatorEnabled(), enableSimulator);
		}


};

QTEST_GUILESS_MAIN(test_UrlUtil)
#include "test_UrlUtil.moc"
