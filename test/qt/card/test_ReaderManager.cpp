/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "MockReaderManagerPlugIn.h"
#include "ReaderManagerWorker.h"

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
		QTime mDieTime;

		void wait(int pMillis = 1000)
		{
			mDieTime = QTime::currentTime().addMSecs(pMillis);
			while (QTime::currentTime() < mDieTime)
			{
				QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
			}
		}


	public Q_SLOTS:
		void onCardCommandDone(QSharedPointer<CreateCardConnectionCommand> pCommand)
		{
			mDieTime = QTime::currentTime();
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
			ReaderManager::getInstance().init();
			ReaderManager::getInstance().getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			ReaderManager::getInstance().shutdown();
		}


		void fireReaderAdded()
		{
			QSignalSpy spy(&ReaderManager::getInstance(), &ReaderManager::fireReaderAdded);

			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			spy.wait();
			QCOMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toString(), QString("MockReader 4711"));
		}


		void fireReaderRemoved()
		{
			QSignalSpy spy(&ReaderManager::getInstance(), &ReaderManager::fireReaderRemoved);
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			MockReaderManagerPlugIn::getInstance().removeReader("MockReader 4711");

			spy.wait();
			QCOMPARE(spy.count(), 1);
			QCOMPARE(spy.takeFirst().at(0).toString(), QString("MockReader 4711"));
		}


		void fireCreateCardConnection_forUnknownReader()
		{
			CreateCardConnectionCommandSlot commandSlot;

			ReaderManager::getInstance().callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			commandSlot.wait();
			QVERIFY(commandSlot.mSlotCalled);
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_noCard()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");

			ReaderManager::getInstance().callCreateCardConnectionCommand("UnknownReader", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			commandSlot.wait();
			QVERIFY(commandSlot.mSlotCalled);
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection_cardConnectFail()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");
			MockCardConfig cardConfig;
			cardConfig.mConnect = CardReturnCode::COMMAND_FAILED;
			reader->setCard(cardConfig);

			ReaderManager::getInstance().callCreateCardConnectionCommand("MockReader 4711", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			commandSlot.wait();
			QVERIFY(commandSlot.mSlotCalled);
			QVERIFY(commandSlot.mCardConnection.isNull());
		}


		void fireCreateCardConnection()
		{
			CreateCardConnectionCommandSlot commandSlot;
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 4711");
			MockCardConfig cardConfig;
			cardConfig.mConnect = CardReturnCode::OK;
			reader->setCard(cardConfig);

			ReaderManager::getInstance().callCreateCardConnectionCommand("MockReader 4711", &commandSlot, &CreateCardConnectionCommandSlot::onCardCommandDone);

			commandSlot.wait();
			QVERIFY(commandSlot.mSlotCalled);
			QVERIFY(!commandSlot.mCardConnection.isNull());
		}


		void getInvalidReaderInfoWithAndWithoutInitializedReaderManager()
		{
			{
				const auto& readerInfo = ReaderManager::getInstance().getReaderInfo("test dummy");
				QCOMPARE(readerInfo.getPlugInType(), ReaderManagerPlugInType::UNKNOWN);
				QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
			}

			cleanupTestCase();
			{
				const auto& readerInfo = ReaderManager::getInstance().getReaderInfo("test dummy");
				QCOMPARE(readerInfo.getPlugInType(), ReaderManagerPlugInType::UNKNOWN);
				QCOMPARE(readerInfo.getName(), QStringLiteral("test dummy"));
			}
			initTestCase();
		}


};

QTEST_GUILESS_MAIN(test_ReaderManager)
#include "test_ReaderManager.moc"
