/*!
 * \brief Unit tests for \ref ProviderConfigurationParser
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
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
										 "         \"shortName\": {\"\" : \"Selbstauskunft\"},"
										 "         \"longName\": {\"\" : \"Selbstauskunft - \\\"Meine Daten einsehen\\\"\"},"
										 "         \"shortDescription\": {\"\" : \"Funktion der AusweisApp2\"},"
										 "         \"longDescription\": {\"\" : \"Die AusweisApp2 verfuegt ueber die Funktion \\\"Meine Daten einsehen\\\"\"},"
										 "         \"address\": \"https://www.ausweisapp.bund.de/online-ausweisen/meine-daten-auslesen/\","
										 "         \"homepage\": \"https://www.ausweisapp.bund.de/\","
										 "         \"phone\": \"+49 421 - 204 95 995\","
										 "         \"email\": \"support@ausweisapp.de\","
										 "         \"postalAddress\": \"Governikus GmbH & Co. KG<br/>- im Auftrag des Bundesamtes fuer Sicherheit in der Informationstechnik -<br/>Hochschulring 4<br/>D-28359 Bremen\","
										 "         \"category\": \"citizen\""
										 "      },"
										 "		{"
										 "         \"shortName\": {\"\" : \"Selbstauskunft\"},"
										 "         \"longName\": {\"\" : \"\"},"
										 "         \"shortDescription\": {\"\" : \"Funktion der AusweisApp2\"},"
										 "         \"longDescription\": {\"\" : \"Die AusweisApp2 verfuegt ueber die Funktion \\\"Meine Daten einsehen\\\"\"},"
										 "         \"address\": \"https://www.ausweisapp.bund.de/online-ausweisen/meine-daten-auslesen/\","
										 "         \"homepage\": \"https://www.ausweisapp.bund.de/\","
										 "         \"phone\": \"+49 421 - 204 95 995\","
										 "         \"email\": \"support@ausweisapp.de\","
										 "         \"postalAddress\": \"Governikus GmbH & Co. KG<br/>- im Auftrag des Bundesamtes fuer Sicherheit in der Informationstechnik -<br/>Hochschulring 4<br/>D-28359 Bremen\","
										 "         \"category\": \"citizen\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 3);

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

			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)-fr-kasten"));
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
			QCOMPARE(provider.getShortName().toString(), QStringLiteral("Selbstauskunft"));
			QCOMPARE(provider.getLongName().toString(), QStringLiteral("Selbstauskunft - \"Meine Daten einsehen\""));
			QCOMPARE(provider.getShortDescription().toString(), QStringLiteral("Funktion der AusweisApp2"));
			QCOMPARE(provider.getLongDescription().toString(), QStringLiteral("Die AusweisApp2 verfuegt ueber die Funktion \"Meine Daten einsehen\""));
			QCOMPARE(provider.getAddress(), QStringLiteral("https://www.ausweisapp.bund.de/online-ausweisen/meine-daten-auslesen/"));
			QCOMPARE(provider.getHomepage(), QStringLiteral("https://www.ausweisapp.bund.de/"));
			QCOMPARE(provider.getPhone(), QStringLiteral("+49 421 - 204 95 995"));
			QCOMPARE(provider.getEMail(), QStringLiteral("support@ausweisapp.de"));
			QCOMPARE(provider.getPostalAddress(), QStringLiteral("Governikus GmbH & Co. KG<br/>- im Auftrag des Bundesamtes fuer Sicherheit in der Informationstechnik -<br/>Hochschulring 4<br/>D-28359 Bremen"));
			QCOMPARE(provider.getCategory(), QStringLiteral("citizen"));
			QVERIFY(provider.getIcon()->lookupPath().endsWith("/citizen_button.svg"));
			QVERIFY(provider.getImage()->lookupPath().endsWith("/citizen_bg.svg"));

			provider = providers[2];
			QCOMPARE(provider.getLongName().toString(), QString());
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
										 "         \"subjectUrls\": [\"https://www.autentapp.de/bla1\"]"
										 "      },"
										 "      {"
										 "         \"subjectUrls\": [\"https://www.autentapp.de/bla1\", \"https://www.autentapp.de/bla2\"]"
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
			QCOMPARE(provider.getSubjectUrls(), QStringList({QStringLiteral("https://www.autentapp.de/bla1")}));

			provider = providers[3];
			QCOMPARE(provider.getSubjectUrls(), QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla2")}));
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

			const int desktop = 85;
			QTest::newRow("win") << desktop;
			QTest::newRow("mac") << desktop;
			QTest::newRow("linux") << desktop;
			QTest::newRow("android") << desktop;
			QTest::newRow("ios") << 49;
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
