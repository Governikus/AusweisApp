/*!
 * \brief Unit tests for \ref Reader
 *
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "Reader.h"

#include "MockCardConnectionWorker.h"
#include "MockReader.h"
#include "TestFileHelper.h"

#include <QtTest>


using namespace governikus;


class test_Reader
	: public QObject
{
	Q_OBJECT
	QSharedPointer<MockReader> mReader;
	QString mReaderName = QString("test reader");
	QSharedPointer<MockCardConnectionWorker> mWorker;

	private Q_SLOTS:
		void init()
		{
			mReader.reset(new MockReader(mReaderName));
			mWorker.reset(new MockCardConnectionWorker(mReader.data()));
		}


		void cleanup()
		{
			mReader.clear();
			mWorker.clear();
		}


		void test_CreateCardConnectionWorkerNoCard()
		{
			QTest::ignoreMessage(QtWarningMsg, "No card available");
			QCOMPARE(mReader->createCardConnectionWorker(), QSharedPointer<CardConnectionWorker>());
		}


		void test_CreateCardConnectionWorkerConnected()
		{
			MockCard* card = mReader->setCard(MockCardConfig(), QSharedPointer<EFCardAccess>());
			card->setConnected(true);

			QTest::ignoreMessage(QtWarningMsg, "Card is already connected");
			QCOMPARE(mReader->createCardConnectionWorker(), QSharedPointer<CardConnectionWorker>());
		}


		void test_UpdateRetryCounterCommandFailed()
		{
			CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);

			QTest::ignoreMessage(QtCriticalMsg, "Cannot get EF.CardAccess");
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::COMMAND_FAILED);
		}


		void test_UpdateRetryCounterUnknown()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"));
			auto efCardAccess = EFCardAccess::decode(bytes);
			CardInfo cInfo(CardType::UNKNOWN, efCardAccess, 3, false, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);
			mWorker->addResponse(CardReturnCode::UNKNOWN);
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::UNKNOWN);
		}


		void test_UpdateRetryCounter_OK_RetryCounterChanged()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"));
			auto efCardAccess = EFCardAccess::decode(bytes);
			CardInfo cInfo(CardType::UNKNOWN, efCardAccess, 2, true, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			QSignalSpy spy(mReader.data(), &Reader::fireCardRetryCounterChanged);

			QTest::ignoreMessage(QtDebugMsg, "StatusCode: SUCCESS");
			QTest::ignoreMessage(QtInfoMsg, "retrieved retry counter: 3 , was: 2 , PIN deactivated: false");
			QTest::ignoreMessage(QtDebugMsg, "fireCardRetryCounterChanged");
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::OK);
			QCOMPARE(mReader->getReaderInfo().getRetryCounter(), 3);
			QVERIFY(!mReader->getReaderInfo().isPinDeactivated());
			QCOMPARE(spy.count(), 1);
		}


		void test_Update()
		{
			CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);

			QSignalSpy spyCardInserted(mReader.data(), &Reader::fireCardInserted);
			QSignalSpy spyCardRemoved(mReader.data(), &Reader::fireCardRemoved);

			mReader->update();
			QCOMPARE(spyCardInserted.count(), 0);
			QCOMPARE(spyCardRemoved.count(), 0);

			mReader->setCardEvent(Reader::CardEvent::CARD_INSERTED);
			QTest::ignoreMessage(QtInfoMsg, "Card inserted: {Type: UNKNOWN, Retry counter: 3, Pin deactivated: false}");
			mReader->update();
			QCOMPARE(spyCardInserted.count(), 1);

			mReader->setCardEvent(Reader::CardEvent::CARD_REMOVED);
			QTest::ignoreMessage(QtInfoMsg, "Card removed");
			mReader->update();
			QCOMPARE(spyCardRemoved.count(), 1);
		}


};


QTEST_GUILESS_MAIN(test_Reader)
#include "test_Reader.moc"
