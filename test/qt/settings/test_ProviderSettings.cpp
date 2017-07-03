/*!
 * \brief Unit tests for \ref ProviderSettings
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include <QCoreApplication>
#include <QFile>
#include <QtTest>

#include "ProviderSettings.h"

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
				/* image */ QStringLiteral("images/iOS/Header-Ausweisapp@3x.png"));

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
		}


};

QTEST_GUILESS_MAIN(test_ProviderSettings)
#include "test_ProviderSettings.moc"
