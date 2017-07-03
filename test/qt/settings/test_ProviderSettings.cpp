/*!
 * \brief Unit tests for \ref ProviderSettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ProviderSettings.h"

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

using namespace governikus;


class test_ProviderSettings
	: public QObject
{
	Q_OBJECT
	QScopedPointer<ProviderSettings> settings;

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
			settings.reset(new ProviderSettings());
		}


		void testEquals()
		{
			QScopedPointer<ProviderSettings> otherSettings(new ProviderSettings());
			otherSettings->load();

			QVERIFY(*settings == *otherSettings);

			settings.reset(new ProviderSettings(settings->getIssueDate().addDays(1), settings->getProviders()));
			QVERIFY(*settings != *otherSettings);
			otherSettings.reset(new ProviderSettings(settings->getIssueDate(), settings->getProviders()));
			QVERIFY(*settings == *otherSettings);

			QVector<Provider> providers(settings->getProviders());
			providers.append(Provider(QStringLiteral("prov"), QStringLiteral("http://address.de")));
			settings.reset(new ProviderSettings(settings->getIssueDate(), providers));
			QVERIFY(*settings != *otherSettings);
			otherSettings.reset(new ProviderSettings(settings->getIssueDate(), settings->getProviders()));
			QVERIFY(*settings == *otherSettings);

			providers.removeAt(providers.size() - 1);
			providers.append(Provider(QStringLiteral("prov2"), QStringLiteral("http://address2.de")));
			settings.reset(new ProviderSettings(settings->getIssueDate(), providers));
			QVERIFY(*settings != *otherSettings);
			otherSettings.reset(new ProviderSettings(settings->getIssueDate(), settings->getProviders()));
			QVERIFY(*settings == *otherSettings);
		}


		void testDate()
		{
			QDateTime initial = settings->getIssueDate();
			QDateTime newValue = QDateTime::currentDateTime().addDays(1);

			QEXPECT_FAIL("", "needs to be different", Continue);
			QCOMPARE(settings->getIssueDate(), newValue);

			settings.reset(new ProviderSettings(newValue, settings->getProviders()));
			QCOMPARE(settings->getIssueDate(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings.reset(new ProviderSettings(initial, settings->getProviders()));
			QCOMPARE(settings->getIssueDate(), initial);
			settings->save();
		}


		void testProviders()
		{
			QVector<Provider> initial = settings->getProviders();
			QVector<Provider> newValue(initial);
			newValue.append(Provider(QStringLiteral("name"), QStringLiteral("https://address.de")));

			settings.reset(new ProviderSettings(settings->getIssueDate(), newValue));
			QCOMPARE(settings->getProviders(), newValue);
			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			settings.reset(new ProviderSettings(settings->getIssueDate(), initial));
			QCOMPARE(settings->getProviders(), initial);
			settings->save();
		}


		void testProviderUrls()
		{
			const Provider provider1(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* short description */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("ftp://homepage.com/form"),
				/* homepage */ QStringLiteral("ftp://www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider1.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider1.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const Provider provider2(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* short description */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("https://homepage.com/form"),
				/* homepage */ QStringLiteral("https://www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider2.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider2.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const Provider provider3(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* short description */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("homepage.com/form"),
				/* homepage */ QStringLiteral("www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider3.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider3.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const Provider provider4;
			QCOMPARE(provider4.getAddressDomain(), QString());
			QCOMPARE(provider4.getHomepageBase(), QString());
		}


		void testProviderFields()
		{
			settings.reset(new ProviderSettings(settings->getIssueDate(), QVector<Provider>()));
			settings->save();
			QVERIFY(!settings->isUnsaved());

			// Add image and icon.
			const Provider provider(
				/* short name  */ QStringLiteral("Provider 1"),
				/* long name  */ QStringLiteral("Provider 1 - long name"),
				/* short description */ QStringLiteral("Provider description short"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("Category A"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QStringLiteral("images/npa.ico"),
				/* image */ QStringLiteral("images/iOS/Header-Ausweisapp@3x.png"),
				/* tcTokenUrl */ QStringLiteral("https://npa.allianz.de/azservice/NpaEIDService/nparef/-wnf"),
				/* clientUrl */ QStringLiteral("https://www.bva.bund.de/bafoeg-online/Bafoeg/flow/anmeld"),
				/* subjectUrls */ QStringList({QStringLiteral("https://npa.allianz.de/bla1"), QStringLiteral("https://npa.allianz.de/bla1")})
				);

			const QVector<Provider> providers = {
				provider
			};
			settings.reset(new ProviderSettings(settings->getIssueDate(), providers));

			QVERIFY(settings->isUnsaved());
			settings->save();
			QVERIFY(!settings->isUnsaved());

			QScopedPointer<ProviderSettings> otherSettings(new ProviderSettings());
			otherSettings->load();
			const QVector<Provider> otherProviders = otherSettings->getProviders();
			QCOMPARE(otherProviders.size(), 1);

			const Provider otherProvider = otherProviders.at(0);
			QCOMPARE(otherProvider.getShortName().toString(), QStringLiteral("Provider 1"));
			QCOMPARE(otherProvider.getLongName().toString(), QStringLiteral("Provider 1 - long name"));
			QCOMPARE(otherProvider.getShortDescription().toString(), QStringLiteral("Provider description short"));
			QCOMPARE(otherProvider.getLongDescription().toString(), QStringLiteral("Provider description long"));
			QCOMPARE(otherProvider.getAddress(), QStringLiteral("https://www.homepage.com/form/"));
			QCOMPARE(otherProvider.getHomepage(), QStringLiteral("https://www.homepage.com/"));
			QCOMPARE(otherProvider.getCategory(), QStringLiteral("Category A"));
			QCOMPARE(otherProvider.getPhone(), QStringLiteral("0421 123456"));
			QCOMPARE(otherProvider.getEMail(), QStringLiteral("abc@def.de"));
			QCOMPARE(otherProvider.getPostalAddress(), QStringLiteral("Am Fallturm 9\n28359 Bremen"));
			QCOMPARE(otherProvider.getIcon(), QStringLiteral("images/npa.ico"));
			QCOMPARE(otherProvider.getImage(), QStringLiteral("images/iOS/Header-Ausweisapp@3x.png"));
			QCOMPARE(otherProvider.getTcTokenUrl(), QStringLiteral("https://npa.allianz.de/azservice/NpaEIDService/nparef/-wnf"));
			QCOMPARE(otherProvider.getClientUrl(), QStringLiteral("https://www.bva.bund.de/bafoeg-online/Bafoeg/flow/anmeld"));
			QCOMPARE(otherProvider.getSubjectUrls(), QStringList({QStringLiteral("https://npa.allianz.de/bla1"), QStringLiteral("https://npa.allianz.de/bla1")}));
		}


		void callCostSave()
		{
			QMap<QString, CallCost> callCosts;
			callCosts.insert("1805", CallCost(0, 3.9, 0, 42, 0));
			callCosts.insert("1806", CallCost(0, 4.9, 0, 43, 0));

			settings->setCallCosts(callCosts);
			QVERIFY(settings->isUnsaved());

			settings->save();
			QVERIFY(!settings->isUnsaved());
		}


		void callCostLoad()
		{
			QMap<QString, CallCost> callCosts;
			callCosts.insert("1805", CallCost(0, 3.9, 0, 42, 0));
			callCosts.insert("1806", CallCost(0, 4.9, 0, 43, 0));
			settings->setCallCosts(callCosts);
			settings->save();

			settings.reset(new ProviderSettings());
			settings->load();

			QCOMPARE(settings->mCallCosts, callCosts);
		}


		void getCallCost()
		{
			QMap<QString, CallCost> callCosts;
			callCosts.insert("1805", CallCost(0, 3.9, 0, 42, 0));
			callCosts.insert("1806", CallCost(0, 4.9, 0, 43, 0));
			settings->setCallCosts(callCosts);

			const CallCost& callCost = settings->getCallCost(QStringLiteral("+49 1805-123456789"));

			QVERIFY(!callCost.isNull());
			QCOMPARE(callCost.getFreeSeconds(), 0);
			QCOMPARE(callCost.getLandlineCentsPerMinute(), 3.9);
			QCOMPARE(callCost.getLandlineCentsPerCall(), 0.0);
			QCOMPARE(callCost.getMobileCentsPerMinute(), 42.0);
			QCOMPARE(callCost.getMobileCentsPerCall(), 0.0);
		}


		void getCallCost_withDelimiters()
		{
			QMap<QString, CallCost> callCosts;
			callCosts.insert("1805", CallCost(0, 3.9, 0, 42, 0));
			callCosts.insert("1806", CallCost(0, 4.9, 0, 43, 0));
			settings->setCallCosts(callCosts);

			const CallCost& callCost = settings->getCallCost(QStringLiteral("+49  1-8/05-123456789"));

			QVERIFY(!callCost.isNull());
			QCOMPARE(callCost.getFreeSeconds(), 0);
			QCOMPARE(callCost.getLandlineCentsPerMinute(), 3.9);
			QCOMPARE(callCost.getLandlineCentsPerCall(), 0.0);
			QCOMPARE(callCost.getMobileCentsPerMinute(), 42.0);
			QCOMPARE(callCost.getMobileCentsPerCall(), 0.0);
		}


		void getCallCost_nonExisting()
		{
			QMap<QString, CallCost> callCosts;
			callCosts.insert("1805", CallCost(0, 3.9, 0, 42, 0));
			callCosts.insert("1806", CallCost(0, 4.9, 0, 43, 0));
			settings->setCallCosts(callCosts);

			const CallCost& callCost = settings->getCallCost(QStringLiteral("123456"));

			QVERIFY(callCost.isNull());
		}


};

QTEST_GUILESS_MAIN(test_ProviderSettings)
#include "test_ProviderSettings.moc"
