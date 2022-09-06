/*!
 * \brief Unit tests for \ref ProviderConfigurationParser
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ProviderConfigurationParser.h"

#include "ResourceLoader.h"
#include "TestFileHelper.h"

#include <QtTest>

using namespace governikus;


Q_DECLARE_METATYPE(QLatin1String)
Q_DECLARE_METATYPE(QOperatingSystemVersion::OSType)


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
										 "         \"longName\": {\"\" : \":::(bit)kasten\", \"de\" : \":::(bit)-de-kasten\", \"fr_FR\" : \":::(bit)-fr-kasten\"},"
										 "         \"address\": \"https://www.bitkasten.de/\","
										 "         \"phone\": \"\","
										 "         \"email\": \"\","
										 "         \"postaladdress\": \"\","
										 "         \"category\": \"other\""
										 "      },"
										 "      {"
										 "         \"longName\": {\"\" : \"Selbstauskunft - \\\"Meine Daten einsehen\\\"\"},"
										 "         \"shortName\": {\"\" : \"Selbstauskunft\"},"
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
										 "         \"longName\": {\"\" : \"Selbstauskunft\"},"
										 "         \"shortName\": {\"\" : \"\"},"
										 "         \"shortDescription\": {\"\" : \"Funktion der AusweisApp2\"},"
										 "         \"longDescription\": {\"\" : \"Die AusweisApp2 verfuegt ueber die Funktion \\\"Meine Daten einsehen\\\"\"},"
										 "         \"address\": \"https://www.ausweisapp.bund.de/online-ausweisen/meine-daten-auslesen/\","
										 "         \"homepage\": \"https://www.ausweisapp.bund.de/\","
										 "         \"phone\": \"+49 421 - 204 95 995\","
										 "         \"email\": \"support@ausweisapp.de\","
										 "         \"postalAddress\": \"Governikus GmbH & Co. KG<br/>- im Auftrag des Bundesamtes fuer Sicherheit in der Informationstechnik -<br/>Hochschulring 4<br/>D-28359 Bremen\","
										 "         \"category\": \"citizen\","
										 "         \"internalId\": \"myId\""
										 "      }"
										 "   ]"
										 "}");

			const auto providers = ProviderConfigurationParser::parseProvider(data);

			QCOMPARE(providers.size(), 3);

			auto provider = providers[0];
			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("om");
			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("de");
			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)-de-kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("fr");
			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)-fr-kasten"));

			LanguageLoader::getInstance().mUsedLocale = QLocale("fr_FR");
			QCOMPARE(provider.getLongName().toString(), QStringLiteral(":::(bit)-fr-kasten"));

			QCOMPARE(provider.getShortName().toString(), QStringLiteral(":::(bit)-fr-kasten"));
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
			QCOMPARE(provider.getLongDescription().toString(), QStringLiteral("Die AusweisApp2 verfuegt ueber die Funktion \"Meine Daten einsehen\""));
			QCOMPARE(provider.getAddress(), QStringLiteral("https://www.ausweisapp.bund.de/online-ausweisen/meine-daten-auslesen/"));
			QCOMPARE(provider.getHomepage(), QStringLiteral("https://www.ausweisapp.bund.de/"));
			QCOMPARE(provider.getPhone(), QStringLiteral("+49 421 - 204 95 995"));
			QCOMPARE(provider.getEMail(), QStringLiteral("support@ausweisapp.de"));
			QCOMPARE(provider.getPostalAddress(), QStringLiteral("Governikus GmbH & Co. KG<br/>- im Auftrag des Bundesamtes fuer Sicherheit in der Informationstechnik -<br/>Hochschulring 4<br/>D-28359 Bremen"));
			QCOMPARE(provider.getCategory(), QStringLiteral("citizen"));
			QVERIFY(provider.getIcon()->lookupPath().endsWith("/citizen_button.svg"));
			QVERIFY(provider.getImage()->lookupPath().endsWith("/citizen_bg.svg"));
			QVERIFY(provider.getInternalId().isEmpty());

			provider = providers[2];
			QCOMPARE(provider.getShortName().toString(), QString());
			QCOMPARE(provider.getLongName().toString(), QStringLiteral("Selbstauskunft"));
			QCOMPARE(provider.getInternalId(), QStringLiteral("myId"));
		}


		void parseAdditionalData()
		{
			QByteArray data = QByteArray("{"
										 "   \"provider\": ["
										 "      {"
										 "         \"longName\": {\"\" : \":::(bit)kasten\"},"
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
			QCOMPARE(provider.getLongName().toString(), QString(":::(bit)kasten"));
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
			QTest::addColumn<QOperatingSystemVersion::OSType>("osType");
			QTest::addColumn<int>("majorVersion");
			QTest::addColumn<int>("count");

			const int all = 99;
			const int withEidSupport = 76;
			QTest::newRow("win") << QOperatingSystemVersion::Windows << -1 << all;
			QTest::newRow("mac") << QOperatingSystemVersion::MacOS << -1 << all;
			QTest::newRow("linux") << QOperatingSystemVersion::Unknown << -1 << all;
			QTest::newRow("android") << QOperatingSystemVersion::Android << -1 << withEidSupport;
			QTest::newRow("android-11") << QOperatingSystemVersion::Android << 11 << all;
			QTest::newRow("android-12") << QOperatingSystemVersion::Android << 12 << withEidSupport;
			QTest::newRow("ios") << QOperatingSystemVersion::IOS << -1 << withEidSupport;
		}


		void platformCount()
		{
			QFETCH(QOperatingSystemVersion::OSType, osType);
			QFETCH(int, majorVersion);
			QFETCH(int, count);

			QByteArray data = TestFileHelper::readFile(QStringLiteral(":/updatable-files/supported-providers.json"));
			const auto providers = ProviderConfigurationParser::parseProvider(data, QOperatingSystemVersion(osType, majorVersion));
			QCOMPARE(providers.size(), count);
		}


};

QTEST_GUILESS_MAIN(test_ProviderConfigurationParser)
#include "test_ProviderConfigurationParser.moc"
