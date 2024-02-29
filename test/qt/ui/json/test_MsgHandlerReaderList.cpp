/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerReaderList
 */

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "messages/MsgHandlerReader.h"

#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_MsgHandlerReaderList
	: public QObject
{
	Q_OBJECT

#if __has_include("SmartManager.h")
	const QByteArray mEidType = QByteArray(R"("eidType":"CARD_CERTIFIED",)");
#else
	const QByteArray mEidType;
#endif

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


		void apiLevel1Reader()
		{
			MessageDispatcher dispatcher;
			const QByteArray msg(R"({"cmd": "GET_READER_LIST"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"msg":"READER_LIST","readers":[]})"));

			const QByteArray msgLevel = (R"({"cmd": "SET_API_LEVEL", "level": 1})");
			QCOMPARE(dispatcher.processCommand(msgLevel), QByteArray(R"({"current":1,"msg":"API_LEVEL"})"));
			QCOMPARE(dispatcher.processCommand(msg), QByteArray(R"({"msg":"READER_LIST","reader":[]})"));
		}


		void noReader()
		{
			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER_LIST"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"readers\":[]}"));
		}


		void oneReaderWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);

			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER_LIST"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"readers\":[{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"name\":\"MockReader 0815\"}]}"));
		}


		void oneReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER_LIST"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"readers\":[{\"attached\":true,\"card\":{\"deactivated\":false," + mEidType + "\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"name\":\"MockReader 0815\"}]}"));
		}


		void multipleReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);
			reader->setCard(MockCardConfig());
			reader = MockReaderManagerPlugIn::getInstance().addReader("ReaderMock"_L1);
			reader->setCard(MockCardConfig());
			MockReaderManagerPlugIn::getInstance().addReader("ReaderMockXYZ"_L1);

			reader = MockReaderManagerPlugIn::getInstance().addReader("SpecialMock"_L1);
			reader->setCard(MockCardConfig());
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::UNKNOWN));
			reader->setReaderInfo(info);

			reader = MockReaderManagerPlugIn::getInstance().addReader("SpecialMockWithGermanCard"_L1);
			reader->setCard(MockCardConfig());
			auto cardInfo = CardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);

			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER_LIST"})");

			QByteArray expected("{\"msg\":\"READER_LIST\",\"readers\":["
								"{\"attached\":true,\"card\":{\"deactivated\":false,<EID_TYPE>\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"name\":\"MockReader 0815\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":false,<EID_TYPE>\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"name\":\"ReaderMock\"},"
								"{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"name\":\"ReaderMockXYZ\"},"
								"{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"name\":\"SpecialMock\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":true,<EID_TYPE>\"inoperative\":false,\"retryCounter\":3},\"insertable\":false,\"keypad\":false,\"name\":\"SpecialMockWithGermanCard\"}"
								"]}");
			expected.replace("<EID_TYPE>", mEidType);

			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerReaderList)
#include "test_MsgHandlerReaderList.moc"
