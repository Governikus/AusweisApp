/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "MockReaderManagerPlugin.h"

#include <QCoreApplication>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QThread>
#include <QtTest>


Q_IMPORT_PLUGIN(MockReaderManagerPlugin)


using namespace Qt::Literals::StringLiterals;
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
			QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

			auto* readerManager = Env::getSingleton<ReaderManager>();
			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void cleanup()
		{
			MockReaderManagerPlugin::getInstance().removeAllReader();
		}


		void fireReaderAdded()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);

			MockReaderManagerPlugin::getInstance().addReader("MockReader 4711"_L1);

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			const auto info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));
			QCOMPARE(info.getName(), "MockReader 4711"_L1);
		}


		void fireReaderRemoved()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved);
			fireReaderAdded();

			MockReaderManagerPlugin::getInstance().removeReader("MockReader 4711"_L1);

			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			const auto info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));
			QCOMPARE(info.getName(), "MockReader 4711"_L1);
		}


		void callExecute()
		{
			auto* readerManager = Env::getSingleton<ReaderManager>();

			QTest::ignoreMessage(QtDebugMsg, "Name of the main thread: MainThread");
			qDebug().noquote() << "Name of the main thread:" << QThread::currentThread()->objectName();

			QTest::ignoreMessage(QtDebugMsg, "callExecute from MainThread. Executed in ReaderManagerThread");
			QTest::ignoreMessage(QtDebugMsg, "callExecute from ReaderManagerThread. Executed in ReaderManagerThread");
			readerManager->callExecute([readerManager] {
						qDebug().noquote() << "callExecute from MainThread. Executed in" << QThread::currentThread()->objectName();

						readerManager->callExecute([] {
							qDebug().noquote() << "callExecute from ReaderManagerThread. Executed in" << QThread::currentThread()->objectName();
						});
					});

			readerManager->shutdown();
			QTest::ignoreMessage(QtWarningMsg, "Cannot call Execute if ReaderManager-Thread is not active");
			readerManager->callExecute([] {
						qDebug() << "callExecute without ReaderManagerThread";
					});

			QSignalSpy spy(readerManager, &ReaderManager::fireInitialized);
			readerManager->init();
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
		}


		void fireCreateCardConnection_forUnknownReader()
		{
			CreateCardConnectionCommandSlot commandSlot;

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader"_L1, &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_noCard()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReaderManagerPlugin::getInstance().addReader("MockReader 4711"_L1);

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader"_L1, &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_cardConnectFail()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugin::getInstance().addReader("MockReader 4711"_L1);
			MockCardConfig cardConfig;
			cardConfig.mConnect = CardReturnCode::COMMAND_FAILED;
			reader->setCard(cardConfig);

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("MockReader 4711"_L1, &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugin::getInstance().addReader("MockReader 4711"_L1);
			QList<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6982")));
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("9000")));
			MockCardConfig cardConfig(transmitConfigs);
			cardConfig.mConnect = CardReturnCode::OK;
			reader->setCard(cardConfig, QSharedPointer<EFCardAccess>(), CardType::EID_CARD, FileRef::appEId());

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("MockReader 4711"_L1, &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(!commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnectionFailedSelectApplication()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugin::getInstance().addReader("MockReader 4711"_L1);
			QList<TransmitConfig> transmitConfigs;
			transmitConfigs.append(TransmitConfig(CardReturnCode::OK, QByteArray::fromHex("6A82")));
			MockCardConfig cardConfig(transmitConfigs);
			cardConfig.mConnect = CardReturnCode::OK;
			reader->setCard(cardConfig, QSharedPointer<EFCardAccess>(), CardType::EID_CARD, FileRef::appEId());

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("MockReader 4711"_L1, &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true); // clazy:exclude=qstring-allocations
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void getInvalidReaderInfo()
		{
			const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo("test dummy"_L1);
			QCOMPARE(readerInfo.getPluginType(), ReaderManagerPluginType::UNKNOWN);
			QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
		}


		void checkNoReaderFilter()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);
			const ReaderFilter filter;

			auto readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 0);

			MockReaderManagerPlugin::getInstance().addReader("MockReader 1"_L1);
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);

			MockReaderManagerPlugin::getInstance().addReader("MockReader 2"_L1);
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

			MockReaderManagerPlugin::getInstance().addReader("MockReader same"_L1);
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);

			MockReaderManagerPlugin::getInstance().addReader("MockReader same"_L1);
			QTRY_COMPARE(spy.count(), 2); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 1);
		}


		void checkTypeReaderFilter_data()
		{
			QTest::addColumn<QList<ReaderManagerPluginType>>("types");
			QTest::addColumn<int>("onceCount");
			QTest::addColumn<int>("twiceCount");

			QTest::newRow("0") << QList<ReaderManagerPluginType>({}) << 0 << 0;
			QTest::newRow("1_existing") << QList<ReaderManagerPluginType>({ReaderManagerPluginType::MOCK}) << 1 << 2;
			QTest::newRow("1_non_existing") << QList<ReaderManagerPluginType>({ReaderManagerPluginType::REMOTE_IFD}) << 0 << 0;
			QTest::newRow("2_existing_non_existing") << QList<ReaderManagerPluginType>({ReaderManagerPluginType::MOCK, ReaderManagerPluginType::PCSC}) << 1 << 2;
		}


		void checkTypeReaderFilter()
		{
			QFETCH(QList<ReaderManagerPluginType>, types);
			QFETCH(int, onceCount);
			QFETCH(int, twiceCount);

			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);
			ReaderFilter filter(types);

			auto readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), 0);

			MockReaderManagerPlugin::getInstance().addReader("MockReader 1"_L1);
			QTRY_COMPARE(spy.count(), 1); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), onceCount);

			MockReaderManagerPlugin::getInstance().addReader("MockReader 2"_L1);
			QTRY_COMPARE(spy.count(), 2); // clazy:exclude=qstring-allocations
			readerInfos = Env::getSingleton<ReaderManager>()->getReaderInfos(filter);
			QCOMPARE(readerInfos.count(), twiceCount);
		}


		void test_shelve()
		{
			MockReader* reader = MockReaderManagerPlugin::getInstance().addReader("MockReader"_L1);
			reader->setInfoCardInfo(CardInfo(CardType::EID_CARD));

			QSignalSpy removed(reader, &Reader::fireCardRemoved);
			QSignalSpy properties(reader, &Reader::fireReaderPropertiesUpdated);

			QVERIFY(reader->getReaderInfo().hasCard());
			QVERIFY(!reader->getReaderInfo().isInsertable());

			MockReaderManagerPlugin::getInstance().shelve();
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

			MockReaderManagerPlugin::getInstance().shelve();
			QCOMPARE(removed.count(), 1);
			QCOMPARE(properties.count(), 1);
			QVERIFY(!reader->getReaderInfo().hasCard());
			QVERIFY(reader->getReaderInfo().isInsertable());
		}


};

QTEST_GUILESS_MAIN(test_ReaderManager)
#include "test_ReaderManager.moc"
