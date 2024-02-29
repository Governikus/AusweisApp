/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref MsgHandlerReader
 */

#include "messages/MsgHandlerReader.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "TestFileHelper.h"

#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

Q_DECLARE_METATYPE(ReaderInfo)

class test_MsgHandlerReader
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


		void ctor()
		{
			QSignalSpy spy(&MockReaderManagerPlugIn::getInstance(), &ReaderManagerPlugIn::fireReaderAdded);
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);
			QCOMPARE(spy.count(), 1);
			ReaderInfo info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));

			MsgHandlerReader noReader(ReaderInfo("MockReader"_L1));
			QCOMPARE(noReader.toJson(), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader\"}"));

			MsgHandlerReader reader(info);
			QCOMPARE(reader.toJson(), QByteArray("{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
		}


		void error()
		{
			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Name cannot be undefined\",\"msg\":\"READER\"}"));

			msg = R"({"cmd": "GET_READER", "name": 5})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid name\",\"msg\":\"READER\"}"));
		}


		void oneReaderWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);

			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER", "name": "MockReader 081"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader 081\"}"));

			msg = R"({"cmd": "GET_READER", "name": "MockReader 0815"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));

			MockReaderManagerPlugIn::getInstance().removeReader("MockReader 0815"_L1);
			msg = R"({"cmd": "GET_READER", "name": "MockReader 0815"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
		}


		void oneReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			QByteArray msg(R"({"cmd": "GET_READER", "name": "MockReader 0815"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false," + mEidType + "\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
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
			QByteArray msg(R"({"cmd": "GET_READER", "name": "MockReader 0815"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false," + mEidType + "\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));

			msg = R"({"cmd": "GET_READER", "name": "ReaderMock"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false," + mEidType + "\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"ReaderMock\"}"));

			msg = R"({"cmd": "GET_READER", "name": "ReaderMockXYZ"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"ReaderMockXYZ\"}"));

			msg = R"({"cmd": "GET_READER", "name": "SpecialMock"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"SpecialMock\"}"));

			msg = R"({"cmd": "GET_READER", "name": "SpecialMockWithGermanCard"})";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":true," + mEidType + "\"inoperative\":false,\"retryCounter\":3},\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"SpecialMockWithGermanCard\"}"));
		}


#if __has_include("SmartManager.h")
		void eidType_data()
		{
			QTest::addColumn<QByteArray>("efCardAccess");
			QTest::addColumn<QByteArray>("eidType");

			QTest::newRow("CARD_CERTIFIED") << TestFileHelper::readFile(":/card/efCardAccess.hex"_L1) << QByteArray("CARD_CERTIFIED");
			QTest::newRow("SE_ENDORSED") << TestFileHelper::readFile(":/card/smartEfCardAccess.hex"_L1) << QByteArray("SE_ENDORSED");
		}


		void eidType()
		{
			QFETCH(QByteArray, efCardAccess);
			QFETCH(QByteArray, eidType);

			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815"_L1);
			reader->setCard(MockCardConfig(), EFCardAccess::decode(QByteArray::fromHex(efCardAccess)), CardType::SMART_EID);
			MessageDispatcher dispatcher;

			QByteArray msg(R"({"cmd": "GET_READER", "name": "MockReader 0815"})");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false,\"eidType\":\"<EID_TYPE>\",\"inoperative\":false,\"retryCounter\":-1},\"insertable\":false,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}").replace("<EID_TYPE>", eidType));
		}


#endif

};

QTEST_GUILESS_MAIN(test_MsgHandlerReader)
#include "test_MsgHandlerReader.moc"
