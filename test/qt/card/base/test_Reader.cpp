/*!
 * \brief Unit tests for \ref Reader
 *
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "Reader.h"

#include "MockCardConnectionWorker.h"
#include "MockReader.h"

#include <QtTest>


using namespace governikus;


class test_Reader
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_CreateCardConnectionWorkerNoCard()
		{
			const QString name = QStringLiteral("name");
			MockReader reader(name);

			QTest::ignoreMessage(QtWarningMsg, "No card available");
			QCOMPARE(reader.createCardConnectionWorker(), QSharedPointer<CardConnectionWorker>());
		}


		void test_CreateCardConnectionWorkerConnected()
		{
			const QString name = QStringLiteral("name");
			MockReader reader(name);
			MockCard* card = reader.setCard(MockCardConfig(), QSharedPointer<EFCardAccess>());
			card->setConnected(true);

			QTest::ignoreMessage(QtWarningMsg, "Card is already connected");
			QCOMPARE(reader.createCardConnectionWorker(), QSharedPointer<CardConnectionWorker>());
		}


		void test_UpdateRetryCounterCommandFailed()
		{
			const QString name = QStringLiteral("name");
			MockReader reader(name);
			CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo(name, ReaderManagerPlugInType::UNKNOWN, cInfo);
			reader.mReaderInfo = rInfo;
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			QCOMPARE(reader.updateRetryCounter(worker), CardReturnCode::COMMAND_FAILED);
		}


		void test_GetRetryCounterNoEfCardAccess()
		{
			const QString name = QStringLiteral("name");
			MockReader reader(name);
			CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo(name, ReaderManagerPlugInType::UNKNOWN, cInfo);
			reader.mReaderInfo = rInfo;
			QSharedPointer<MockCardConnectionWorker> worker(new MockCardConnectionWorker());

			QTest::ignoreMessage(QtCriticalMsg, "Cannot get EF.CardAccess");
			int counter = 3;
			bool deactivated = false;
			QCOMPARE(reader.getRetryCounter(worker, counter, deactivated), CardReturnCode::COMMAND_FAILED);
		}


		void test_FireUpdateSignal()
		{
			const QString name = QStringLiteral("name");
			MockReader reader(name);
			CardInfo cInfo(CardType::UNKNOWN, QSharedPointer<const EFCardAccess>(), 3, false, false);
			ReaderInfo rInfo(name, ReaderManagerPlugInType::UNKNOWN, cInfo);
			reader.mReaderInfo = rInfo;

			QSignalSpy spyCardInserted(&reader, &Reader::fireCardInserted);
			QSignalSpy spyCardRemoved(&reader, &Reader::fireCardRemoved);

			reader.fireUpdateSignal(Reader::CardEvent::NONE);
			QCOMPARE(spyCardInserted.count(), 0);
			QCOMPARE(spyCardRemoved.count(), 0);

			QTest::ignoreMessage(QtInfoMsg, "Card inserted: {Type: UNKNOWN, Retry counter: 3, Pin deactivated: false}");
			reader.fireUpdateSignal(Reader::CardEvent::CARD_INSERTED);
			QCOMPARE(spyCardInserted.count(), 1);

			QTest::ignoreMessage(QtInfoMsg, "Card removed");
			reader.fireUpdateSignal(Reader::CardEvent::CARD_REMOVED);
			QCOMPARE(spyCardRemoved.count(), 1);
		}


};


QTEST_GUILESS_MAIN(test_Reader)
#include "test_Reader.moc"
