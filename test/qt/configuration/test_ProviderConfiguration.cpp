/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref ProviderConfiguration
 */

#include "ProviderConfiguration.h"

#include "ResourceLoader.h"

#include <QtTest>

using namespace governikus;

Q_DECLARE_METATYPE(ProviderConfigurationInfo)

class test_ProviderConfiguration
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			ResourceLoader::getInstance().init();
		}


		void testProviderUrls()
		{
			const ProviderConfigurationInfo provider1(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("ftp://homepage.com/form"),
				/* homepage */ QStringLiteral("ftp://www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider1.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider1.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const ProviderConfigurationInfo provider2(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("https://homepage.com/form"),
				/* homepage */ QStringLiteral("https://www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider2.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider2.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const ProviderConfigurationInfo provider3(
				/* short name  */ QString(),
				/* long name  */ QString(),
				/* long description */ QString(),
				/* address */ QStringLiteral("homepage.com/form"),
				/* homepage */ QStringLiteral("www.homepage.de/bla/bla1")
				);
			QCOMPARE(provider3.getAddressDomain(), QStringLiteral("homepage.com"));
			QCOMPARE(provider3.getHomepageBase(), QStringLiteral("www.homepage.de"));

			const ProviderConfigurationInfo provider4;
			QCOMPARE(provider4.getAddressDomain(), QString());
			QCOMPARE(provider4.getHomepageBase(), QString());
		}


		void testProviderFields()
		{
			// Add image and icon.
			const ProviderConfigurationInfo provider(
				/* short name  */ QStringLiteral("Provider 1"),
				/* long name  */ QStringLiteral("Provider 1 - long name"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryA"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")})
				);

			QCOMPARE(provider.getShortName().toString(), QStringLiteral("Provider 1"));
			QCOMPARE(provider.getLongName().toString(), QStringLiteral("Provider 1 - long name"));
			QCOMPARE(provider.getLongDescription().toString(), QStringLiteral("Provider description long"));
			QCOMPARE(provider.getAddress(), QStringLiteral("https://www.homepage.com/form/"));
			QCOMPARE(provider.getHomepage(), QStringLiteral("https://www.homepage.com/"));
			QCOMPARE(provider.getCategory(), QStringLiteral("CategoryA"));
			QCOMPARE(provider.getPhone(), QStringLiteral("0421 123456"));
			QCOMPARE(provider.getEMail(), QStringLiteral("abc@def.de"));
			QCOMPARE(provider.getPostalAddress(), QStringLiteral("Am Fallturm 9\n28359 Bremen"));
			QVERIFY(provider.getIcon()->lookupPath().endsWith("/CategoryA_button.svg"));
			QVERIFY(provider.getImage()->lookupPath().endsWith("/CategoryA_bg.svg"));
			QCOMPARE(provider.getSubjectUrls(), QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")}));
		}


		void checkName()
		{
			const ProviderConfigurationInfo providerEmptyLongname(
				/* short name  */ QStringLiteral("Provider 1"),
				/* long name  */ QString(QLatin1String("")),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryA"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")}));

			QCOMPARE(providerEmptyLongname.getLongName().toString(), QString());

			const ProviderConfigurationInfo providerWithoutShortname(
				/* short name  */ QString(),
				/* long name  */ QStringLiteral("Provider 1"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryA"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")}));

			QCOMPARE(providerWithoutShortname.getShortName().toString(), QStringLiteral("Provider 1"));
		}


		void checkPhone()
		{
			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				QVERIFY(provider.getPhone().isEmpty() || provider.getPhone().startsWith("+49 "));
			}
		}


		void checkCallCost_data()
		{
			QTest::addColumn<QString>("phone");
			QTest::addColumn<bool>("notNull");
			QTest::addColumn<int>("freeSeconds");
			QTest::addColumn<double>("landlineCentsPerMinute");
			QTest::addColumn<double>("landlineCentsPerCall");
			QTest::addColumn<double>("mobileCentsPerMinute");
			QTest::addColumn<double>("mobileCentsPerCall");

			QTest::newRow("delimiter") << "+49  1-8/05-123456789" << true << 0 << 14.0 << 0.0 << 42.0 << 0.0;
			QTest::newRow("notExisting") << "+49 123456" << false << 0 << 0.0 << 0.0 << 0.0 << 0.0;
			QTest::newRow("01") << "+49 1371" << true << 0 << 0.0 << 14.0 << 0.0 << 0.0;
			QTest::newRow("02") << "+49 1372" << true << 0 << 14.0 << 0.0 << 0.0 << 0.0;
			QTest::newRow("03") << "+49 1376" << true << 0 << 0.0 << 25.0 << 0.0 << 0.0;
			QTest::newRow("04") << "+49 1377" << true << 0 << 0.0 << 100.0 << 0.0 << 0.0;
			QTest::newRow("05") << "+49 1378" << true << 0 << 0.0 << 50.0 << 0.0 << 0.0;
			QTest::newRow("06") << "+49 1801" << true << 0 << 3.9 << 0.0 << 42.0 << 0.0;
			QTest::newRow("07") << "+49 1802" << true << 0 << 0.0 << 6.0 << 42.0 << 0.0;
			QTest::newRow("08") << "+49 1803" << true << 0 << 9.0 << 0.0 << 42.0 << 0.0;
			QTest::newRow("09") << "+49 1804" << true << 0 << 0.0 << 20.0 << 42.0 << 0.0;
			QTest::newRow("10") << "+49 1805" << true << 0 << 14.0 << 0.0 << 42.0 << 0.0;
			QTest::newRow("11") << "+49 1806" << true << 0 << 0.0 << 20.0 << 0.0 << 60.0;
			QTest::newRow("12") << "+49 1807" << true << 30 << 14.0 << 0.0 << 42.0 << 0.0;
		}


		void checkCallCost()
		{
			QFETCH(QString, phone);
			QFETCH(bool, notNull);
			QFETCH(int, freeSeconds);
			QFETCH(double, landlineCentsPerMinute);
			QFETCH(double, landlineCentsPerCall);
			QFETCH(double, mobileCentsPerMinute);
			QFETCH(double, mobileCentsPerCall);
			const ProviderConfigurationInfo provider(QString(), QString(), QString(), QString(), QString(), QString(""), phone);
			const CallCost& callCost = Env::getSingleton<ProviderConfiguration>()->getCallCost(provider);

			QVERIFY(notNull || callCost.isNull());
			QCOMPARE(callCost.getFreeSeconds(), freeSeconds);
			QCOMPARE(callCost.getLandlineCentsPerMinute(), landlineCentsPerMinute);
			QCOMPARE(callCost.getLandlineCentsPerCall(), landlineCentsPerCall);
			QCOMPARE(callCost.getMobileCentsPerMinute(), mobileCentsPerMinute);
			QCOMPARE(callCost.getMobileCentsPerCall(), mobileCentsPerCall);
		}


		void testProviderHosts_data()
		{
			QTest::addColumn<ProviderConfigurationInfo>("provider");

			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				QTest::newRow(provider.getShortName().toString().toUtf8().constData()) << provider;
			}

		}


		void testProviderHosts()
		{
			QFETCH(ProviderConfigurationInfo, provider);

			const auto& address = QUrl(provider.getAddress());
			QVERIFY(!address.host().isEmpty());
			QCOMPARE(address.scheme(), QString("https"));

			const auto& homepage = QUrl(provider.getHomepage());
			QVERIFY(!homepage.host().isEmpty());
			QCOMPARE(homepage.scheme(), QString("https"));

			const auto& subjectUrls = provider.getSubjectUrls();
			for (const auto& url : subjectUrls)
			{
				if (!url.isEmpty())
				{
					const auto& subjectUrl = QUrl(url);
					QVERIFY(!subjectUrl.host().isEmpty());
					QCOMPARE(subjectUrl.scheme(), QString("https"));
				}
			}
		}


		void testProvidersAreEqual()
		{
			const ProviderConfigurationInfo provider1(
				/* short name  */ QStringLiteral("Provider"),
				/* long name  */ QStringLiteral("Provider - long name"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryA"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")})
				);

			const ProviderConfigurationInfo provider2(
				/* short name  */ QStringLiteral("Provider"),
				/* long name  */ QStringLiteral("Provider - long name"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryB"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")})
				);

			const ProviderConfigurationInfo provider3(
				/* short name  */ QStringLiteral("Provider"),
				/* long name  */ QStringLiteral("Provider - long name"),
				/* long description */ QStringLiteral("Provider description long"),
				/* address */ QStringLiteral("https://www.homepage.com/form/"),
				/* homepage */ QStringLiteral("https://www.homepage.com/"),
				/* category */ QStringLiteral("CategoryB"),
				/* phone */ QStringLiteral("0421 123456"),
				/* email */ QStringLiteral("abc@def.de"),
				/* postal address */ QStringLiteral("Am Fallturm 9\n28359 Bremen"),
				/* icon */ QString(),
				/* image */ QString(),
				/* subjectUrls */ QStringList({QStringLiteral("https://www.autentapp.de/bla1"), QStringLiteral("https://www.autentapp.de/bla1")})
				);

			QVERIFY(provider1 == provider1);
			QVERIFY(!(provider1 == provider2));
			QVERIFY(provider2 == provider3);
		}


		void testCheckAttachedEidServices()
		{
			uint attachedEidCounter = 0;

			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				const auto& urls = provider.getSubjectUrls();
				for (const auto& urlString : urls)
				{
					const auto& subjectHost = QUrl(urlString).host();
					if (subjectHost == provider.getAddressDomain() || subjectHost == provider.getHomepageBase())
					{
						attachedEidCounter++;
					}
				}
			}

			QCOMPARE(attachedEidCounter, 19);
		}


		void testCheckServicesWithoutAnOwnEidServer()
		{
			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				QVERIFY2(!provider.getSubjectUrls().isEmpty() || !provider.getSubjectUrlInfo().isEmpty(), provider.getShortName().toString().toStdString().c_str());
			}
		}


		void testCheckUniqueSubjectUrls()
		{
			QSet<QString> subjectUrls;
			uint subjectUrlCounter = 0;

			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				const auto& urls = provider.getSubjectUrls();
				for (const auto& urlString : urls)
				{
					QUrl url(urlString);
					QVERIFY(url.isValid());
					QCOMPARE(urlString, QString("https://%1").arg(url.host()));

					subjectUrls += urlString;
					subjectUrlCounter++;
				}
			}

			QCOMPARE(subjectUrls.size(), subjectUrlCounter);
		}


		void testCheckUrlFormat()
		{
			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			for (const auto& provider : providers)
			{
				QVERIFY(QUrl(provider.getAddress()).isValid());
				QVERIFY(!provider.getAddress().endsWith('/'));
				QVERIFY(!provider.getAddress().endsWith('#'));
				QVERIFY(QUrl(provider.getHomepage()).isValid());
				QVERIFY(!provider.getHomepage().endsWith('/'));
				QVERIFY(!provider.getHomepage().endsWith('#'));
			}
		}


		void testInternalIds()
		{
			const QStringList internalIds = {"pinResetService"};
			const auto& providerConfiguration = Env::getSingleton<ProviderConfiguration>();

			const ProviderConfigurationInfo empty;
			QVERIFY(providerConfiguration->getProviderInfo(QStringLiteral("nonExistentId")) == empty);
			for (const auto& id : internalIds)
			{
				QVERIFY2(providerConfiguration->getProviderInfo(id) != empty, qPrintable(QString("Internal ID %1 is not present").arg(id)));
			}

			for (const auto& info : providerConfiguration->getProviderConfigurationInfos())
			{
				const auto& id = info.getInternalId();
				if (!id.isEmpty())
				{
					QVERIFY2(internalIds.contains(id), qPrintable(QString("Internal ID %1 is not known to test").arg(id)));
				}
			}
		}


		void testInternalIdsUniqueness()
		{
			const auto& providers = Env::getSingleton<ProviderConfiguration>()->getProviderConfigurationInfos();
			QStringList internalIds;

			for (const auto& info : providers)
			{
				const auto& id = info.getInternalId();
				if (!id.isEmpty())
				{
					QVERIFY2(!internalIds.contains(id), qPrintable(QString("Internal IDs must be unique, %1 already present").arg(id)));
					internalIds << id;
				}
			}
		}


};

QTEST_GUILESS_MAIN(test_ProviderConfiguration)
#include "test_ProviderConfiguration.moc"
