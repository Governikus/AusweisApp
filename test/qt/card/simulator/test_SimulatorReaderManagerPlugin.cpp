/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "SimulatorReaderManagerPlugin.h"

#include "AppSettings.h"
#include "VolatileSettings.h"

#include <QtTest>


using namespace Qt::Literals::StringLiterals;
using namespace governikus;


constexpr QLatin1StringView readerName("Simulator");


class test_SimulatorReaderManagerPlugin
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void cleanup()
		{
			Env::getSingleton<VolatileSettings>()->setUsedAsSDK(true);
			Env::getSingleton<AppSettings>()->getSimulatorSettings().setEnabled(false);
		}


		void initialization()
		{
			SimulatorReaderManagerPlugin plugin;
			QSignalSpy spyStatus(&plugin, &ReaderManagerPlugin::fireStatusChanged);

			QCOMPARE(plugin.getInfo().getPluginType(), ReaderManagerPluginType::SIMULATOR);
			QCOMPARE(plugin.getInfo().isAvailable(), true);
			QCOMPARE(plugin.getInfo().isEnabled(), false);

			QTest::ignoreMessage(QtDebugMsg, "SimulatorStateChanged: true");
			plugin.init();
			QCOMPARE(spyStatus.count(), 1);
			QCOMPARE(plugin.getInfo().isEnabled(), true);
		}


		void getReader()
		{
			SimulatorReaderManagerPlugin plugin;
			QCOMPARE(plugin.getReader(readerName), nullptr);

			plugin.init();
			QCOMPARE(plugin.getReader(readerName), nullptr);

			plugin.startScan(true);
			QVERIFY(plugin.getReader(readerName));
			QCOMPARE(plugin.getReader("wrongName"_L1), nullptr);
		}


		void settings()
		{
			SimulatorReaderManagerPlugin plugin;
			QSignalSpy spyAdded(&plugin, &SimulatorReaderManagerPlugin::fireReaderAdded);
			QSignalSpy spyRemoved(&plugin, &SimulatorReaderManagerPlugin::fireReaderRemoved);
			auto& simulatorSettings = Env::getSingleton<AppSettings>()->getSimulatorSettings();
			QCOMPARE(simulatorSettings.isEnabled(), false);
			auto* volatileSettings = Env::getSingleton<VolatileSettings>();
			QCOMPARE(volatileSettings->isUsedAsSDK(), true);

			volatileSettings->setUsedAsSDK(false);
			QCOMPARE(simulatorSettings.isEnabled(), false);

			plugin.init();
			QCOMPARE(simulatorSettings.isEnabled(), false);

			QTest::ignoreMessage(QtDebugMsg, "SimulatorStateChanged: true");
			simulatorSettings.setEnabled(true);
			QCOMPARE(spyAdded.count(), 0);

			QTest::ignoreMessage(QtDebugMsg, "SimulatorStateChanged: false");
			simulatorSettings.setEnabled(false);
			QCOMPARE(spyRemoved.count(), 0);

			simulatorSettings.setEnabled(true);
			plugin.startScan(true);
			QCOMPARE(spyAdded.count(), 1);

			simulatorSettings.setEnabled(false);
			QCOMPARE(spyRemoved.count(), 1);
		}


		void shelve()
		{
			SimulatorReaderManagerPlugin plugin;
			plugin.shelveAll();

			plugin.init();
			plugin.shelveAll();

			QTest::ignoreMessage(QtDebugMsg, "Card shelved");
			plugin.startScan(true);

			const auto& reader = plugin.getReader(readerName);
			QSignalSpy spyInserted(reader, &Reader::fireCardInserted);
			QSignalSpy spyRemoved(reader, &Reader::fireCardRemoved);
			plugin.insert(readerName, QVariant());
			QCOMPARE(spyInserted.count(), 1);

			QTest::ignoreMessage(QtDebugMsg, "Card shelved");
			plugin.shelveAll();
			QCOMPARE(spyRemoved.count(), 1);
		}


};

QTEST_GUILESS_MAIN(test_SimulatorReaderManagerPlugin)
#include "test_SimulatorReaderManagerPlugin.moc"
