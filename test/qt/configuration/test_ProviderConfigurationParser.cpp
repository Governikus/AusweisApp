/*!
 * \brief Unit tests for \ref ProviderConfigurationParser
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationParser.h"

#include "ResourceLoader.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(QLatin1String)


class test_ProviderConfigurationParser
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void parseEmpty()
		{
			QByteArray data = QByteArray("{}");

			QCOMPARE(ProviderConfigurationParser::parseCallCosts(data).size(), 0);
			QCOMPARE(ProviderConfigurationParser::parseProvider(data).size(), 0);
		}


		void parseProviders()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "         \"shortName\": {\"\" : \":::(bit)kasten\", \"de\" : \":::(bit)-de-kasten\", \"fr_FR\" : \":::(bit)-fr-kasten\"},"
										 "         \"address\": \"https://www.bitkasten.de/\","
										 "         \"phone\": \"\","
										 "         \"email\": \"\","
										 "         \"postaladdress\": \"\","
										 "         \"category\": \"other\""
										 "      },"
										 "      {"
										 "         \"shortName\": {\"\" : \"Allianz Kundenportal\"},"
										 "         \"longName\": {\"\" : \"Allianz Kundenportal - Meine Allianz\"},"
										 "         \"shortDescription\": {\"\" : \"Kundenportal von Allianz\"},"
										 "         \"longDescription\": {\"\" : \"Kundenportal von Allianz - Meine Allianz\"},"
										 "         \"address\": \"https://meine.allianz.de/form\","
										 "         \"homepage\": \"https://meine.allianz.de\","
										 "         \"phone\": \"0421 123456\","
										 "         \"email\": \"abc@def.de\","
										 "         \"postalAddress\": \"Am Fallturm 9,\n28359 Bremen\","
										 "         \"category\": \"insurance\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 2);

			auto provider = providers[0];
			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("om");
			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("de");
			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)-de-kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("fr");
			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)-fr-kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("fr_FR");
			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)-fr-kasten"));

			QCOMPARE(provider.getLongName().toString(), QString());
			QCOMPARE(provider.getShortDescription().toString(), QString());
			QCOMPARE(provider.getLongDescription().toString(), QString());
			QCOMPARE(provider.getAddress(), QStringLiteral("https://www.bitkasten.de/"));
			QCOMPARE(provider.getHomepage(), QString());
			QCOMPARE(provider.getPhone(), QString());
			QCOMPARE(provider.getEMail(), QString());
			QCOMPARE(provider.getPostalAddress(), QString());
			QCOMPARE(provider.getCategory(), QStringLiteral("other"));
			QVERIFY(provider.getIcon()->lookupPath().endsWith("/other_button.svg"));
			QVERIFY(provider.getImage()->lookupPath().endsWith("/other_bg.svg"));

			provider = providers[1];
			QCOMPARE(provider.getShortName().toString(), QStringLiteral("Allianz Kundenportal"));
			QCOMPARE(provider.getLongName().toString(), QStringLiteral("Allianz Kundenportal - Meine Allianz"));
			QCOMPARE(provider.getShortDescription().toString(), QStringLiteral("Kundenportal von Allianz"));
			QCOMPARE(provider.getLongDescription().toString(), QStringLiteral("Kundenportal von Allianz - Meine Allianz"));
			QCOMPARE(provider.getAddress(), QStringLiteral("https://meine.allianz.de/form"));
			QCOMPARE(provider.getHomepage(), QStringLiteral("https://meine.allianz.de"));
			QCOMPARE(provider.getPhone(), QStringLiteral("0421 123456"));
			QCOMPARE(provider.getEMail(), QStringLiteral("abc@def.de"));
			QCOMPARE(provider.getPostalAddress(), QStringLiteral("Am Fallturm 9,\n28359 Bremen"));
			QCOMPARE(provider.getCategory(), QStringLiteral("insurance"));
			QVERIFY(provider.getIcon()->lookupPath().endsWith("/insurance_button.svg"));
			QVERIFY(provider.getImage()->lookupPath().endsWith("/insurance_bg.svg"));
		}


		void parseAdditionalData()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "         \"shortName\": {\"\" : \":::(bit)kasten\"},"
										 "         \"address\": \"https://www.bitkasten.de/\","
										 "         \"phone\": \"\","
										 "         \"email\": \"\","
										 "         \"postaladdress\": \"\","
										 "         \"category\": \"other\","
										 "         \"someNewProperty\": \"blabla\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 1);
			auto provider = providers[0];
			QCOMPARE(provider.getShortName().toString(), QString(":::(bit)kasten"));
			QCOMPARE(provider.getAddress(), QString("https://www.bitkasten.de/"));
			QCOMPARE(provider.getPhone(), QString(""));
			QCOMPARE(provider.getEMail(), QString(""));
			QCOMPARE(provider.getPostalAddress(), QString(""));
			QCOMPARE(provider.getCategory(), QString("other"));
		}


		void parseTcTokenUrl()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "      },"
										 "      {"
										 "         \"tcTokenUrl\": \"https://npa.allianz.de/azservice/NpaEIDService/nparef/-wnf\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 2);

			auto provider = providers[0];
			QCOMPARE(provider.getTcTokenUrl(), QUrl());

			provider = providers[1];
			QCOMPARE(provider.getTcTokenUrl(), QUrl(QStringLiteral("https://npa.allianz.de/azservice/NpaEIDService/nparef/-wnf")));
		}


		void parseClientUrl()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "      },"
										 "      {"
										 "         \"clientUrl\": \"https://www.bva.bund.de/bafoeg-online/Bafoeg/flow/anmeld\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 2);

			auto provider = providers[0];
			QCOMPARE(provider.getTcTokenUrl(), QUrl());

			provider = providers[1];
			QCOMPARE(provider.getClientUrl(), QUrl(QStringLiteral("https://www.bva.bund.de/bafoeg-online/Bafoeg/flow/anmeld")));
		}


		void parseSubjectUrls()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "      },"
										 "      {"
										 "         \"subjectUrls\": []"
										 "      },"
										 "      {"
										 "         \"subjectUrls\": [\"https://npa.allianz.de/bla1\"]"
										 "      },"
										 "      {"
										 "         \"subjectUrls\": [\"https://npa.allianz.de/bla1\", \"https://npa.allianz.de/bla2\"]"
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 4);

			auto provider = providers[0];
			QCOMPARE(provider.getSubjectUrls(), QStringList());

			provider = providers[1];
			QCOMPARE(provider.getSubjectUrls(), QStringList());

			provider = providers[2];
			QCOMPARE(provider.getSubjectUrls(), QStringList({QStringLiteral("https://npa.allianz.de/bla1")}));

			provider = providers[3];
			QCOMPARE(provider.getSubjectUrls(), QStringList({QStringLiteral("https://npa.allianz.de/bla1"), QStringLiteral("https://npa.allianz.de/bla2")}));
		}


		void defaultProviders()
		{
			QByteArray data = TestFileHelper::readFile(QStringLiteral(":/updatable-files/supported-providers.json"));

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QVERIFY(!providers.isEmpty());
		}


		void parseCallCosts()
		{
			QByteArray data = TestFileHelper::readFile(QStringLiteral(":/updatable-files/supported-providers.json"));

			const auto callCosts = ProviderConfigurationParser::parseCallCosts(data);

			QCOMPARE(callCosts.size(), 17);
		}


		void platformCount_data()
		{
			QTest::addColumn<int>("count");

			const int desktop = 59;
			QTest::newRow("win") << desktop;
			QTest::newRow("mac") << desktop;
			QTest::newRow("linux") << desktop;
			QTest::newRow("android") << 54;
			QTest::newRow("ios") << 14;
		}


		void platformCount()
		{
			QFETCH(int, count);

			QByteArray data = TestFileHelper::readFile(QStringLiteral(":/updatable-files/supported-providers.json"));

			const auto providers = ProviderConfigurationParser::parseProvider(data, QLatin1String(QTest::currentDataTag()));

			QCOMPARE(providers.size(), count);
		}


		void checkExcludedPlatform_data()
		{
			QTest::addColumn<QByteArray>("content");
			QTest::addColumn<QLatin1String>("currentOS");
			QTest::addColumn<bool>("excluded");

			QTest::newRow("mobile")
				<< QByteArray(R"(   ["mobile"]   )") << QLatin1String("android") << true;

			QTest::newRow("android")
				<< QByteArray(R"(   ["mac", "android"]   )") << QLatin1String("android") << true;

			QTest::newRow("desktop_excluded_mac")
				<< QByteArray(R"(   ["android", "desktop"]   )") << QLatin1String("mac") << true;

			QTest::newRow("ios")
				<< QByteArray(R"(   ["android", "desktop"]   )") << QLatin1String("ios") << false;

			QTest::newRow("ios")
				<< QByteArray(R"(   ["android", "win"]   )") << QLatin1String("ios") << false;

			QTest::newRow("mobile_excluded")
				<< QByteArray(R"(   ["win", "mobile"]   )") << QLatin1String("android") << true;

			QTest::newRow("mobile_excluded")
				<< QByteArray(R"(   ["mobile", "win"]   )") << QLatin1String("ios") << true;

			QTest::newRow("win_excluded")
				<< QByteArray(R"(   ["win"]   )") << QLatin1String("win") << true;

			QTest::newRow("desktop_excluded_single")
				<< QByteArray(R"(   ["desktop"]   )") << QLatin1String("win") << true;

			QTest::newRow("win")
				<< QByteArray(R"(   ["mac", "bla", "bsd", "linux", "mobile", "ios", "android"]   )") << QLatin1String("win") << false;

			QTest::newRow("desktop_excluded_multi")
				<< QByteArray(R"(   ["mac", "bla", "bsd", "linux", "desktop", "ios", "android"]   )") << QLatin1String("win") << true;

			QTest::newRow("win_excluded")
				<< QByteArray(R"(   ["mac", "bla", "dummy", "win"]   )") << QLatin1String("win") << true;

			QTest::newRow("nothing")
				<< QByteArray(R"(   []   )") << QLatin1String("win") << false;
		}


		void checkExcludedPlatform()
		{
			QFETCH(QByteArray, content);
			QFETCH(QLatin1String, currentOS);
			QFETCH(bool, excluded);

			const auto& doc = QJsonDocument::fromJson(content);
			QVERIFY(!doc.isNull());
			QCOMPARE(ProviderConfigurationParser::isExcludedPlatform(doc.array(), currentOS), excluded);
		}


		void checkExcludedPlatformEmpty()
		{
			QCOMPARE(ProviderConfigurationParser::isExcludedPlatform(QJsonArray(), QLatin1String("ios")), false);
		}


};

QTEST_GUILESS_MAIN(test_ProviderConfigurationParser)
#include "test_ProviderConfigurationParser.moc"
