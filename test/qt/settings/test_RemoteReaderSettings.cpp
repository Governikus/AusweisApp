/*!
 * \brief Unit tests for \ref RemoteReaderSettings
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include <QtTest>

#include "RemoteReaderSettings.h"
#include "TestFileHelper.h"


using namespace governikus;


class test_RemoteReaderSettings
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testStandardValues()
		{
			RemoteReaderSettings settings;

			QCOMPARE(settings.getServerName(), QLatin1String("RemoteReader"));
			QCOMPARE(settings.useEncryption(), false);
			QCOMPARE(settings.getPassword(), QString(""));
		}


		void testServerName()
		{
			RemoteReaderSettings settings;
			QCOMPARE(settings.getServerName(), QLatin1String("RemoteReader"));
			settings.load();
			QCOMPARE(settings.getServerName(), QLatin1String("RemoteReader"));
			settings.save();
			QCOMPARE(settings.getServerName(), QLatin1String("RemoteReader"));
			settings.setServerName(QLatin1String("Google Pixel"));
			QCOMPARE(settings.getServerName(), QLatin1String("Google Pixel"));
			settings.load();
			QCOMPARE(settings.getServerName(), QLatin1String("RemoteReader"));
			settings.setServerName(QLatin1String("Google Pixel"));
			settings.save();
			QCOMPARE(settings.getServerName(), QLatin1String("Google Pixel"));
			settings.load();
			QCOMPARE(settings.getServerName(), QLatin1String("Google Pixel"));
		}


		void testUseEncryption()
		{
			RemoteReaderSettings settings;
			QCOMPARE(settings.useEncryption(), false);
			settings.load();
			QCOMPARE(settings.useEncryption(), false);
			settings.save();
			QCOMPARE(settings.useEncryption(), false);
			settings.setEncryption(true);
			QCOMPARE(settings.useEncryption(), true);
			settings.load();
			QCOMPARE(settings.useEncryption(), false);
			settings.setEncryption(true);
			settings.save();
			QCOMPARE(settings.useEncryption(), true);
			settings.load();
			QCOMPARE(settings.useEncryption(), true);
		}


		void testPassword()
		{
			RemoteReaderSettings settings;
			QCOMPARE(settings.getPassword(), QString(""));
			settings.load();
			QCOMPARE(settings.getPassword(), QString(""));
			settings.save();
			QCOMPARE(settings.getPassword(), QString(""));
			settings.setPassword(QLatin1String("Hallo123"));
			QCOMPARE(settings.getPassword(), QLatin1String("Hallo123"));
			settings.load();
			QCOMPARE(settings.getPassword(), QString(""));
			settings.setPassword(QLatin1String("Hallo123"));
			settings.save();
			QCOMPARE(settings.getPassword(), QLatin1String("Hallo123"));
			settings.load();
			QCOMPARE(settings.getPassword(), QLatin1String("Hallo123"));
		}


		void testEquals()
		{
			RemoteReaderSettings settings1;
			RemoteReaderSettings settings2;
			QVERIFY(settings1 == settings2);
			QVERIFY(settings2 == settings1);

			settings1.setServerName(QLatin1String("Google Pixel"));
			QVERIFY(settings1 != settings2);
			QVERIFY(settings2 != settings1);

			settings2.setServerName(QLatin1String("Google Pixel"));
			QVERIFY(settings1 == settings2);
			QVERIFY(settings2 == settings1);

			settings1.setEncryption(true);
			QVERIFY(settings1 != settings2);
			QVERIFY(settings2 != settings1);

			settings2.setEncryption(true);
			QVERIFY(settings1 == settings2);
			QVERIFY(settings2 == settings1);

			settings1.setPassword(QLatin1String("Hallo123"));
			QVERIFY(settings1 != settings2);
			QVERIFY(settings2 != settings1);

			settings2.setPassword(QLatin1String("Hallo123"));
			QVERIFY(settings1 == settings2);
			QVERIFY(settings2 == settings1);
		}


};

QTEST_GUILESS_MAIN(test_RemoteReaderSettings)
#include "test_RemoteReaderSettings.moc"
