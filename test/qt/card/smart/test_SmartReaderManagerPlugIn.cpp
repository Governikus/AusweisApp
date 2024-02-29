/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "AppSettings.h"
#include "ReaderManager.h"
#include "SmartReaderManagerPlugIn.h"

#include <QSignalSpy>
#include <QtTest>


using namespace governikus;


Q_DECLARE_METATYPE(ReaderInfo)


class test_SmartReaderManagerPlugIn
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void initialization()
		{
			Env::getSingleton<ReaderManager>()->callExecute([]{
					SmartReaderManagerPlugIn plugin;
					plugin.init();
					QCOMPARE(plugin.getInfo().isAvailable(), false);
					QCOMPARE(plugin.getReaders().size(), 0);
				});

		}


		void availableChanged()
		{
			Env::getSingleton<ReaderManager>()->callExecute([]{
					SmartReaderManagerPlugIn plugin;

					QSignalSpy readerAddedSpy(&plugin, &SmartReaderManagerPlugIn::fireReaderAdded);
					QSignalSpy readerUpdatedSpy(&plugin, &SmartReaderManagerPlugIn::fireReaderPropertiesUpdated);
					QSignalSpy readerRemovedSpy(&plugin, &SmartReaderManagerPlugIn::fireReaderRemoved);
					auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();

					QCOMPARE(plugin.getReaders().size(), 0);

					settings.setSmartAvailable(true);
					QCOMPARE(readerAddedSpy.count(), 1);
					const auto& readerAddedInfo = readerAddedSpy.at(0).at(0).value<ReaderInfo>();
					QCOMPARE(readerAddedInfo.getName(), QStringLiteral("Smart"));
					QCOMPARE(readerAddedInfo.isValid(), true);
					QCOMPARE(readerUpdatedSpy.count(), 0);
					QCOMPARE(readerRemovedSpy.count(), 0);
					QCOMPARE(plugin.getReaders().size(), 1);
					QVERIFY(plugin.getReaders().at(0) != nullptr);

					settings.setSmartAvailable(false);
					QCOMPARE(readerAddedSpy.count(), 1);
					QCOMPARE(readerUpdatedSpy.count(), 1);
					const auto& readerUpdatedInfo1 = readerUpdatedSpy.at(0).at(0).value<ReaderInfo>();
					QCOMPARE(readerUpdatedInfo1.getName(), QStringLiteral("Smart"));
					QCOMPARE(readerUpdatedInfo1.isValid(), false);
					QCOMPARE(readerRemovedSpy.count(), 0);
					QCOMPARE(plugin.getReaders().size(), 0);

					settings.setSmartAvailable(true);
					QCOMPARE(readerAddedSpy.count(), 1);
					QCOMPARE(readerUpdatedSpy.count(), 2);
					const auto& readerUpdatedInfo2 = readerUpdatedSpy.at(1).at(0).value<ReaderInfo>();
					QCOMPARE(readerUpdatedInfo2.getName(), QStringLiteral("Smart"));
					QCOMPARE(readerUpdatedInfo2.isValid(), true);
					QCOMPARE(readerRemovedSpy.count(), 0);
					QCOMPARE(plugin.getReaders().size(), 1);
					QVERIFY(plugin.getReaders().at(0) != nullptr);
				});
		}


};

QTEST_GUILESS_MAIN(test_SmartReaderManagerPlugIn)
#include "test_SmartReaderManagerPlugIn.moc"
