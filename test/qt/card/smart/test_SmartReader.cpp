/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include <QtTest>

#include "ReaderManager.h"
#include "SmartReader.h"
#include "mock/eid_applet_interface_mock.h"


using namespace governikus;


class test_SmartReader
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


		void getCard_data()
		{
			QTest::addColumn<EidStatus>("status");

			QTest::newRow("internal_error") << EidStatus::INTERNAL_ERROR;
			QTest::newRow("no_provisioning") << EidStatus::NO_PROVISIONING;
			QTest::newRow("no_personalization") << EidStatus::NO_PERSONALIZATION;
			QTest::newRow("personalized") << EidStatus::PERSONALIZED;
		}


		void getCard()
		{
			QFETCH(EidStatus, status);

			Env::getSingleton<ReaderManager>()->callExecute([status] {
					setSmartEidStatus(status);

					SmartReader reader(QStringLiteral("Smart"));
					QCOMPARE(reader.getCard(), nullptr);
					QCOMPARE(reader.getReaderInfo().isInsertable(), false);

					QSignalSpy spyInserted(&reader, &Reader::fireCardInserted);
					QSignalSpy spyRemoved(&reader, &Reader::fireCardRemoved);

					reader.connectReader();
					QCOMPARE(spyInserted.count(), 0);
					QCOMPARE(spyRemoved.count(), 0);
					QCOMPARE(reader.getCard() != nullptr, (status == EidStatus::PERSONALIZED));
					QCOMPARE(reader.getReaderInfo().isInsertable(), (status == EidStatus::PERSONALIZED));

					reader.disconnectReader(QString());
					QCOMPARE(spyInserted.count(), 0);
					QCOMPARE(spyRemoved.count(), 0);

					auto spyCount = status == EidStatus::PERSONALIZED ? 1 : 0;
					reader.connectReader();

					reader.insertCard();
					QCOMPARE(spyInserted.count(), spyCount);
					QCOMPARE(spyRemoved.count(), 0);
					QCOMPARE(reader.getCard() == nullptr, status != EidStatus::PERSONALIZED);

					reader.disconnectReader(QString());
					QCOMPARE(spyInserted.count(), spyCount);
					QCOMPARE(spyRemoved.count(), spyCount);
				});
		}


};

QTEST_GUILESS_MAIN(test_SmartReader)
#include "test_SmartReader.moc"
