/*!
 * \brief Unit tests for \ref Reader
 *
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
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
			QSignalSpy spy(mReader.data(), &Reader::fireCardInfoChanged);

			QTest::ignoreMessage(QtDebugMsg, "StatusCode: SUCCESS");
			QTest::ignoreMessage(QtInfoMsg, "retrieved retry counter: 3 , was: 2 , PIN deactivated: false , PIN initial:  false ");
			QTest::ignoreMessage(QtDebugMsg, "fireCardInfoChanged");
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::OK);
			QCOMPARE(mReader->getReaderInfo().getRetryCounter(), 3);
			QVERIFY(!mReader->getReaderInfo().isPinDeactivated());
			QCOMPARE(spy.count(), 1);

			const auto& commands = mWorker->getCommands();
			QCOMPARE(commands.size(), 1);
			QCOMPARE(commands.at(0), QByteArray::fromHex("0022C1A412800A04007F0007020204020283010384010D"));
		}


		void test_UpdateRetryCounter_WithoutParameterId()
		{
			QByteArray hexString("31 11"
								 "        30 0F"
								 "            06 0A 04007F00070202040202"
								 "            02 01 02");
			auto efCardAccess = EFCardAccess::fromHex(hexString);
			CardInfo cInfo(CardType::UNKNOWN, efCardAccess, 2, true, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("9000"));
			QSignalSpy spy(mReader.data(), &Reader::fireCardInfoChanged);

			QTest::ignoreMessage(QtDebugMsg, "StatusCode: SUCCESS");
			QTest::ignoreMessage(QtInfoMsg, "retrieved retry counter: 3 , was: 2 , PIN deactivated: false , PIN initial:  false ");
			QTest::ignoreMessage(QtDebugMsg, "fireCardInfoChanged");
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::OK);
			QCOMPARE(mReader->getReaderInfo().getRetryCounter(), 3);
			QVERIFY(!mReader->getReaderInfo().isPinDeactivated());
			QCOMPARE(spy.count(), 1);

			const auto& commands = mWorker->getCommands();
			QCOMPARE(commands.size(), 1);
			QCOMPARE(commands.at(0), QByteArray::fromHex("0022C1A40F800A04007F00070202040202830103"));
		}


		void test_UpdateRetryCounter_InitialPinChanged()
		{
			QByteArray bytes = QByteArray::fromHex(TestFileHelper::readFile(":/card/efCardAccess.hex"));
			auto efCardAccess = EFCardAccess::decode(bytes);
			CardInfo cInfo(CardType::UNKNOWN, efCardAccess, 3, true, false);
			ReaderInfo rInfo(mReaderName, ReaderManagerPlugInType::UNKNOWN, cInfo);
			mReader->setReaderInfo(rInfo);
			mWorker->addResponse(CardReturnCode::OK, QByteArray::fromHex("63D3"));
			QSignalSpy spy(mReader.data(), &Reader::fireCardInfoChanged);

			QTest::ignoreMessage(QtInfoMsg, "retrieved retry counter: 3 , was: 3 , PIN deactivated: false , PIN initial:  true ");
			QTest::ignoreMessage(QtDebugMsg, "fireCardInfoChanged");
			QCOMPARE(mReader->updateRetryCounter(mWorker), CardReturnCode::OK);
			QVERIFY(mReader->getReaderInfo().getCardInfo().isPinInitial());
			QCOMPARE(spy.count(), 1);
		}


		void test_shelve_insert_data()
		{
			QTest::addColumn<CardType>("type");
			QTest::addColumn<int>("retryCounter");
			QTest::addColumn<int>("propertiesCount");
			QTest::addColumn<int>("infoCount");
			QTest::addColumn<bool>("insertable");

			QTest::newRow("NONE") << CardType::NONE << -1 << 0 << 0 << false;
			QTest::newRow("EID_CARD") << CardType::EID_CARD << 3 << 1 << 0 << true;
			QTest::newRow("SMART_EID_RC3") << CardType::SMART_EID << 3 << 1 << 0 << true;
			QTest::newRow("SMART_EID_RC0") << CardType::SMART_EID << 0 << 0 << 1 << false;
		}


		void test_shelve_insert()
		{
			QFETCH(CardType, type);
			QFETCH(int, retryCounter);
			QFETCH(int, propertiesCount);
			QFETCH(int, infoCount);
			QFETCH(bool, insertable);

			QSignalSpy removed(mReader.data(), &Reader::fireCardRemoved);
			QSignalSpy properties(mReader.data(), &Reader::fireReaderPropertiesUpdated);
			QSignalSpy info(mReader.data(), &Reader::fireCardInfoChanged);

			QVERIFY(!mReader->getReaderInfo().isInsertable());
			mReader->setInfoCardInfo(CardInfo(type, nullptr, retryCounter));

			mReader->shelveCard();
			QCOMPARE(removed.count(), 0);
			QCOMPARE(properties.count(), propertiesCount);
			QCOMPARE(info.count(), infoCount);
			QCOMPARE(mReader->getReaderInfo().isInsertable(), insertable);

			if (insertable)
			{
				QSignalSpy inserted(mReader.data(), &Reader::fireCardInserted);

				mReader->insertCard();
				QCOMPARE(inserted.count(), 1);
				QCOMPARE(removed.count(), 0);
				QCOMPARE(properties.count(), propertiesCount);
				QCOMPARE(info.count(), infoCount);
				QVERIFY(mReader->getReaderInfo().isInsertable());

				mReader->shelveCard();
				QCOMPARE(inserted.count(), 1);
				QCOMPARE(removed.count(), 1);
				QCOMPARE(properties.count(), propertiesCount);
				QCOMPARE(info.count(), infoCount);
				QVERIFY(mReader->getReaderInfo().isInsertable());
			}
		}


};


QTEST_GUILESS_MAIN(test_Reader)
#include "test_Reader.moc"
