/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"

#include <QCoreApplication>
#include <QSharedPointer>
#include <QSignalSpy>
#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;


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
			readerManager->getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			Env::getSingleton<ReaderManager>()->shutdown();
		}


		void fireReaderAdded()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderAdded);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toString(), QString("MockReader 4711"));
		}


		void fireReaderRemoved()
		{
			QSignalSpy spy(Env::getSingleton<ReaderManager>(), &ReaderManager::fireReaderRemoved);
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			MockReaderManagerPlugIn::getInstance().removeReader("MockReader 4711");

			QTRY_COMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toString(), QString("MockReader 4711"));
		}


		void fireCreateCardConnection_forUnknownReader()
		{
			CreateCardConnectionCommandSlot commandSlot;

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true);
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_noCard()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			Env::getSingleton<ReaderManager>()->callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			QTRY_COMPARE(commandSlot.mSlotCalled, true);
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

			QTRY_COMPARE(commandSlot.mSlotCalled, true);
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

			QTRY_COMPARE(commandSlot.mSlotCalled, true);
			QVERIFY(!commandSlot.mCardConnection.isNull());
		}


		void getInvalidReaderInfoWithAndWithoutInitializedReaderManager()
		{
			{
				const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo("test dummy");
				QCOMPARE(readerInfo.getPlugInType(), ReaderManagerPlugInType::UNKNOWN);
				QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
			}

			cleanupTestCase();
			{
				const auto& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo("test dummy");
				QCOMPARE(readerInfo.getPlugInType(), ReaderManagerPlugInType::UNKNOWN);
				QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
			}
			initTestCase();
		}


};

QTEST_GUILESS_MAIN(test_ReaderManager)
#include "test_ReaderManager.moc"
