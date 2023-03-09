/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"

#include <QCoreApplication>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

Q_DECLARE_METATYPE(ReaderInfo)

class CreateCardConnectionCommandSlot
	: public QObject
{
	Q_OBJECT

	public:
		QSharedPointer<CardConnection> mCardConnection;
		bool mSlotCalled = false;

	public Q_SLOTS:
		void onCardCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
		{
			mSlotCalled = true;
			mCardConnection = pCommand->getCardConnection();
		}


};


class test_ReaderManager
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			const auto readerManager = Env::getSingleton<ReaderManager>();
			readerManager->init();
			readerManager->isScanRunning(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugIn::getInstance().removeAllReader();
		}


		void fireReaderAdded()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			const auto info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));
			QCOMPARE(info.getName(), QString("MockReader 4711"));
		}


		void fireReaderRemoved()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved);
			fireReaderAdded();

			MockReaderManagerPlugIn::getInstance().removeReader("MockReader 4711");

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			const auto info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));
			QCOMPARE(info.getName(), QString("MockReader 4711"));
		}


		void fireCreateCardConnection_forUnknownReader()
		{
			CreateCardConnectionCommandSlot commandSlot;

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_noCard()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_cardConnectFail()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");
			MockCardConfig cardConfig;
			cardConfig.mConnect = CardReturnCode::COMMAND_FAILED;
			reader->setCard(cardConfig);

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("MockReader 4711", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");
			MockCardConfig cardConfig;
			cardConfig.mConnect = CardReturnCode::OK;
			reader->setCard(cardConfig);

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("MockReader 4711", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(!commandSlot.mCardConnection.isNull());
		}


		void getInvalidReaderInfo()
		{
			const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo("test dummy");
			QCOMPARE(readerInfo.getPlugInType(), ReaderManagerPlugInType::UNKNOWN);
			QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
		}


		void checkNoReaderFilter()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);
			const ReaderFilter filter;

			auto readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 0);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 2");
			QTRY_COMPARE(spy.count(), 2); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 2);
		}


		void checkUniqueReaderFilter()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);
			const ReaderFilter filter(ReaderFilter::UniqueReaderTypes);

			auto readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 0);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader same");
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader same");
			QTRY_COMPARE(spy.count(), 2); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);
		}


		void checkTypeReaderFilter_data()
		{
			QTest::addColumn<QVector<ReaderManagerPlugInType>>("types");
			QTest::addColumn<int>("onceCount");
			QTest::addColumn<int>("twiceCount");

			QTest::newRow("0") << QVector<ReaderManagerPlugInType>({}) << 0 << 0;
			QTest::newRow("1_existing") << QVector<ReaderManagerPlugInType>({ReaderManagerPlugInType::MOCK}) << 1 << 2;
			QTest::newRow("1_non_existing") << QVector<ReaderManagerPlugInType>({ReaderManagerPlugInType::REMOTE_IFD}) << 0 << 0;
			QTest::newRow("2_existing_non_existing") << QVector<ReaderManagerPlugInType>({ReaderManagerPlugInType::MOCK, ReaderManagerPlugInType::PCSC}) << 1 << 2;
		}


		void checkTypeReaderFilter()
		{
			QFETCH(QVector<ReaderManagerPlugInType>, types);
			QFETCH(int, onceCount);
			QFETCH(int, twiceCount);

			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);
			ReaderFilter filter(types);

			auto readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 0);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 1");
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), onceCount);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 2");
			QTRY_COMPARE(spy.count(), 2); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), twiceCount);
		}


		void test_shelve()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader");
			reader->setInfoCardInfo(CardInfo(CardType::EID_CARD));

			QSignalSpy removed(reader, &Reader::fireCardRemoved);
			QSignalSpy properties(reader, &Reader::fireReaderPropertiesUpdated);

			QVERIFY(reader->getReaderInfo().hasCard());
			QVERIFY(!reader->getReaderInfo().isInsertable());

			MockReaderManagerPlugIn::getInstance().shelve();
			QCOMPARE(removed.count(), 0);
			QCOMPARE(properties.count(), 0);
			QVERIFY(reader->getReaderInfo().hasCard());
			QVERIFY(!reader->getReaderInfo().isInsertable());

			reader->shelveCard();
			reader->insertCard();
			QCOMPARE(removed.count(), 0);
			QCOMPARE(properties.count(), 1);
			QVERIFY(reader->getReaderInfo().hasCard());
			QVERIFY(reader->getReaderInfo().isInsertable());

			MockReaderManagerPlugIn::getInstance().shelve();
			QCOMPARE(removed.count(), 1);
			QCOMPARE(properties.count(), 1);
			QVERIFY(!reader->getReaderInfo().hasCard());
			QVERIFY(reader->getReaderInfo().isInsertable());
		}


};

QTEST_GUILESS_MAIN(test_ReaderManager)
#include "test_ReaderManager.moc"
