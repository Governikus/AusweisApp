/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */


#include "SimulatorSettings.h"

#include "AppSettings.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


class test_SimulatorSettings
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			// init ctor of Singleton, otherwise the testcases are not
			// stable. There could be differences between a run of a
			// single testcase and the whole testsuite.
			Q_UNUSED(Env::getSingleton<AppSettings>()->getSimulatorSettings())
		}


		void init()
		{
			AbstractSettings::mTestDir.clear();
		}


		void testEnabled()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getSimulatorSettings();
			QSignalSpy settingsSpy(&settings, &SimulatorSettings::fireEnabledChanged);

			QCOMPARE(settingsSpy.count(), 0);
			QCOMPARE(settings.isEnabled(), false);

			settings.setEnabled(true);
			QCOMPARE(settingsSpy.count(), 1);
			QCOMPARE(settings.isEnabled(), true);

			settings.setEnabled(true);
			QCOMPARE(settingsSpy.count(), 1);
			QCOMPARE(settings.isEnabled(), true);

			settings.setEnabled(false);
			QCOMPARE(settingsSpy.count(), 2);
			QCOMPARE(settings.isEnabled(), false);

			settings.setEnabled(false);
			QCOMPARE(settingsSpy.count(), 2);
			QCOMPARE(settings.isEnabled(), false);
		}


		void testBasicReader()
		{
			auto& settings = Env::getSingleton<AppSettings>()->getSimulatorSettings();
			QCOMPARE(settings.isBasicReader(), false);

			settings.setBasicReader(true);
			QCOMPARE(settings.isBasicReader(), true);

			settings.setBasicReader(true);
			QCOMPARE(settings.isBasicReader(), true);

			settings.setBasicReader(false);
			QCOMPARE(settings.isBasicReader(), false);

			settings.setBasicReader(false);
			QCOMPARE(settings.isBasicReader(), false);
		}


};

QTEST_GUILESS_MAIN(test_SimulatorSettings)
#include "test_SimulatorSettings.moc"
