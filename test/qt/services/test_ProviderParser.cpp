/*!
 * \brief Unit tests for \ref ProviderParser
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProviderParser.h"
#include <QtTest>

#include "TestFileHelper.h"

using namespace governikus;


class test_ProviderParser
	: public QObject
{
	Q_OBJECT

	ProviderParser parser;

	private Q_SLOTS:
		void parseEmpty()
		{
			QByteArray data = QByteArray("{}");

			QSharedPointer<ProviderSettings> providerSettings = parser.parse(data);

			QVERIFY(providerSettings);
			QVERIFY(providerSettings->getIssueDate().isNull());
			QVERIFY(providerSettings->getProviders().isEmpty());
		}


		void parseIssueDate()
		{
			QByteArray data = QByteArray("{"
										 "    \"issueDate\": \"2015-10-29T06:25:00+1:00\"}");

			QSharedPointer<ProviderSettings> providerSettings = parser.parse(data);

			QVERIFY(providerSettings);
			QCOMPARE(providerSettings->getIssueDate(), QDateTime::fromString("2015-10-29T06:25:00+1:00", Qt::ISODate));
			QVERIFY(providerSettings->getProviders().isEmpty());
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
										 "         \"category\": \"insurance\","
										 "         \"icon\": \"qrc:///images/npa.ico\","
										 "         \"image\": \"qrc:///images/iOS/Header-Ausweisapp@3x.png\""
										 "      }"
										 "   ]"
										 "}");

			QSharedPointer<ProviderSettings> providerSettings = parser.parse(data);

			QVERIFY(providerSettings);
			QVERIFY(providerSettings->getIssueDate().isNull());
			QCOMPARE(providerSettings->getProviders().size(), 2);

			auto provider = providerSettings->getProviders()[0];
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
			QCOMPARE(provider.getIcon(), QString());
			QCOMPARE(provider.getImage(), QString());

			provider = providerSettings->getProviders()[1];
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
			QCOMPARE(provider.getIcon(), QStringLiteral("qrc:///images/npa.ico"));
			QCOMPARE(provider.getImage(), QStringLiteral("qrc:///images/iOS/Header-Ausweisapp@3x.png"));
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

			QSharedPointer<ProviderSettings> providerSettings = parser.parse(data);

			QVERIFY(providerSettings);
			QVERIFY(providerSettings->getIssueDate().isNull());
			QCOMPARE(providerSettings->getProviders().size(), 1);
			auto provider = providerSettings->getProviders()[0];
			QCOMPARE(provider.getShortName().toString(), QString(":::(bit)kasten"));
			QCOMPARE(provider.getAddress(), QString("https://www.bitkasten.de/"));
			QCOMPARE(provider.getPhone(), QString(""));
			QCOMPARE(provider.getEMail(), QString(""));
			QCOMPARE(provider.getPostalAddress(), QString(""));
			QCOMPARE(provider.getCategory(), QString("other"));
		}


		void defaultProviders()
		{
			QByteArray data = TestFileHelper::readFile(QCoreApplication::applicationDirPath() + "/default-providers.json");

			QSharedPointer<ProviderSettings> providerSettings = parser.parse(data);

			QVERIFY(providerSettings);
			QVERIFY(providerSettings->getIssueDate().isValid());
			QVERIFY(!providerSettings->getProviders().isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_ProviderParser)
#include "test_ProviderParser.moc"
