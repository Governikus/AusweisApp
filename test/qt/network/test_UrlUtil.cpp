/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref UrlUtil
 */

#include "UrlUtil.h"

#include "AppSettings.h"
#include "Env.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
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

			QTest::newRow("none_80") << QUrl("http://www.web.de/index.html"_L1) << QUrl("http://www.web.de:80"_L1) << true;
			QTest::newRow("8080_8080") << QUrl("http://www.web.de:8080/index.html"_L1) << QUrl("http://www.web.de:8080"_L1) << true;
			QTest::newRow("none_443") << QUrl("https://www.web.de/index.html"_L1) << QUrl("https://www.web.de:443"_L1) << true;
			QTest::newRow("8443_8443") << QUrl("https://www.web.de:8443/index.html"_L1) << QUrl("https://www.web.de:8443"_L1) << true;

			QTest::newRow("false_8443_8444") << QUrl("https://www.web.de:8443/index.html"_L1) << QUrl("https://www.web.de:8444"_L1) << false;
			QTest::newRow("false_999_999") << QUrl("https://www.web.de:999/index.html"_L1) << QUrl("http://www.web.de:999"_L1) << false;
			QTest::newRow("false_different_domain") << QUrl("http://www.google.de:999/index.html"_L1) << QUrl("http://www.web.de:999"_L1) << false;

			QTest::newRow("no_scheme_https_with_port") << QUrl("de.dummy.cz"_L1) << QUrl("https://de.dummy.cz:443"_L1) << false;
			QTest::newRow("no_scheme_https_without_port") << QUrl("de.dummy.cz"_L1) << QUrl("https://de.dummy.cz"_L1) << false;

			QTest::newRow("no_scheme_http_with_port") << QUrl("de.dummy.cz"_L1) << QUrl("http://de.dummy.cz:80"_L1) << false;
			QTest::newRow("no_scheme_http_without_port") << QUrl("de.dummy.cz"_L1) << QUrl("http://de.dummy.cz"_L1) << false;
		}


		void isMatchingSameOriginPolicy()
		{
			QFETCH(QUrl, url1);
			QFETCH(QUrl, url2);
			QFETCH(bool, valid);

			QCOMPARE(UrlUtil::isMatchingSameOriginPolicy(url1, url2), valid);
		}


		void getRequest_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<UrlQueryRequest>("type");
			QTest::addColumn<QLatin1String>("value");

			QTest::newRow("empty") << QUrl(""_L1) << UrlQueryRequest::UNKNOWN << QLatin1String();
			QTest::newRow("no query") << QUrl("example.com"_L1) << UrlQueryRequest::UNKNOWN << QLatin1String();
			QTest::newRow("empty value") << QUrl("?showui="_L1) << UrlQueryRequest::SHOWUI << QLatin1String();
			QTest::newRow("showui - l") << QUrl("?showui=test"_L1) << UrlQueryRequest::SHOWUI << "test"_L1;
			QTest::newRow("showui - h") << QUrl("?SHOWUI=test"_L1) << UrlQueryRequest::SHOWUI << "test"_L1;
			QTest::newRow("status - l") << QUrl("?status=test"_L1) << UrlQueryRequest::STATUS << "test"_L1;
			QTest::newRow("status - h") << QUrl("?STATUS=test"_L1) << UrlQueryRequest::STATUS << "test"_L1;
			QTest::newRow("tctokenurl - l") << QUrl("?tctokenurl=test"_L1) << UrlQueryRequest::TCTOKENURL << "test"_L1;
			QTest::newRow("tctokenurl - h") << QUrl("?TCTOKENURL=test"_L1) << UrlQueryRequest::TCTOKENURL << "test"_L1;
			QTest::newRow("multi 1") << QUrl("?showui=test1&status=test2"_L1) << UrlQueryRequest::SHOWUI << "test1"_L1;
			QTest::newRow("multi 2") << QUrl("?status=test1&showui=test2"_L1) << UrlQueryRequest::STATUS << "test1"_L1;
			QTest::newRow("multi 3") << QUrl("?foo=test1&showui=test2"_L1) << UrlQueryRequest::SHOWUI << "test2"_L1;
			QTest::newRow("multi 4") << QUrl("?foo=&showui=test2"_L1) << UrlQueryRequest::SHOWUI << "test2"_L1;
			QTest::newRow("multi 5") << QUrl("?foo=test1&showui="_L1) << UrlQueryRequest::SHOWUI << QLatin1String();
			QTest::newRow("multi 6") << QUrl("?showui=&showui=test1"_L1) << UrlQueryRequest::SHOWUI << QLatin1String();
			QTest::newRow("multi 7") << QUrl("?foo=test1&bar=test2"_L1) << UrlQueryRequest::UNKNOWN << QLatin1String();
		}


		void getRequest()
		{
			QFETCH(QUrl, url);
			QFETCH(UrlQueryRequest, type);
			QFETCH(QLatin1String, value);

			const auto queryUrl = QUrlQuery(url);
			const auto [parsedType, parsedValue] = UrlUtil::getRequest(queryUrl);
			QCOMPARE(parsedType, type);
			QCOMPARE(parsedValue, value);
		}


		void setHiddenSettings_data()
		{
			QTest::addColumn<QUrl>("url");
			QTest::addColumn<bool>("useTestUri");
			QTest::addColumn<bool>("enableSimulator");

			QTest::newRow("empty") << QUrl(""_L1) << false << false;
			QTest::newRow("useTestUri") << QUrl("?useTestUri=true"_L1) << true << false;
			QTest::newRow("!useTestUri") << QUrl("?useTestUri=false"_L1) << false << false;
			QTest::newRow("enableSimulator") << QUrl("?enableSimulator=true"_L1) << false << true;
			QTest::newRow("!enableSimulator") << QUrl("?enableSimulator=false"_L1) << false << false;
			QTest::newRow("multi 1") << QUrl("?useTestUri=true&enableSimulator=true"_L1) << true << true;
			QTest::newRow("multi 2") << QUrl("?useTestUri=true&enableSimulator=false"_L1) << true << false;
		}


		void setHiddenSettings()
		{
			auto& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
			auto& simulatorSettings = Env::getSingleton<AppSettings>()->getSimulatorSettings();

			generalSettings.setDeveloperOptions(true);
			generalSettings.setUseSelfauthenticationTestUri(false);
			simulatorSettings.setEnabled(false);

			QCOMPARE(generalSettings.useSelfAuthTestUri(), false);
			QCOMPARE(simulatorSettings.isEnabled(), false);

			QFETCH(QUrl, url);
			QFETCH(bool, useTestUri);
			QFETCH(bool, enableSimulator);

			UrlUtil::setHiddenSettings(QUrlQuery(url));

			QCOMPARE(generalSettings.useSelfAuthTestUri(), useTestUri);
			QCOMPARE(simulatorSettings.isEnabled(), enableSimulator);
		}


};

QTEST_GUILESS_MAIN(test_UrlUtil)
#include "test_UrlUtil.moc"
