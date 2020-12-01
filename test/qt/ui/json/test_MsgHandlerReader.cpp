/*!
 * \brief Unit tests for \ref MsgHandlerReader
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "messages/MsgHandlerReader.h"

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"

#include <QtTest>

Q_IMPORT_PLUGIN(MockReaderManagerPlugIn)

using namespace governikus;

Q_DECLARE_METATYPE(ReaderInfo)

class test_MsgHandlerReader
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


		void ctor()
		{
			QSignalSpy spy(&MockReaderManagerPlugIn::getInstance(), &ReaderManagerPlugIn::fireReaderAdded);
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");
			QCOMPARE(spy.count(), 1);
			ReaderInfo info = qvariant_cast<ReaderInfo>(spy.takeFirst().at(0));

			MsgHandlerReader noReader(ReaderInfo("MockReader"));
			QCOMPARE(noReader.toJson(), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader\"}"));

			MsgHandlerReader reader(info);
			QCOMPARE(reader.toJson(), QByteArray("{\"attached\":true,\"card\":null,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
		}


		void error()
		{
			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Name cannot be undefined\",\"msg\":\"READER\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": 5}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"error\":\"Invalid name\",\"msg\":\"READER\"}"));
		}


		void oneReaderWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER\", \"name\": \"MockReader 081\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader 081\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": \"MockReader 0815\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));

			MockReaderManagerPlugIn::getInstance().removeReader("MockReader 0815");
			msg = "{\"cmd\": \"GET_READER\", \"name\": \"MockReader 0815\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
		}


		void oneReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER\", \"name\": \"MockReader 0815\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));
		}


		void multipleReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");
			reader->setCard(MockCardConfig());
			reader = MockReaderManagerPlugIn::getInstance().addReader("ReaderMock");
			reader->setCard(MockCardConfig());
			MockReaderManagerPlugIn::getInstance().addReader("ReaderMockXYZ");

			reader = MockReaderManagerPlugIn::getInstance().addReader("SpecialMock");
			reader->setCard(MockCardConfig());
			ReaderInfo info = reader->getReaderInfo();
			info.setCardInfo(CardInfo(CardType::UNKNOWN));
			reader->setReaderInfo(info);

			reader = MockReaderManagerPlugIn::getInstance().addReader("SpecialMockWithGermanCard");
			reader->setCard(MockCardConfig());
			auto cardInfo = CardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			info = reader->getReaderInfo();
			info.setCardInfo(cardInfo);
			reader->setReaderInfo(info);

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER\", \"name\": \"MockReader 0815\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"msg\":\"READER\",\"name\":\"MockReader 0815\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": \"ReaderMock\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"msg\":\"READER\",\"name\":\"ReaderMock\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": \"ReaderMockXYZ\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"keypad\":false,\"msg\":\"READER\",\"name\":\"ReaderMockXYZ\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": \"SpecialMock\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":null,\"keypad\":false,\"msg\":\"READER\",\"name\":\"SpecialMock\"}"));

			msg = "{\"cmd\": \"GET_READER\", \"name\": \"SpecialMockWithGermanCard\"}";
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"attached\":true,\"card\":{\"deactivated\":true,\"inoperative\":false,\"retryCounter\":3},\"keypad\":false,\"msg\":\"READER\",\"name\":\"SpecialMockWithGermanCard\"}"));
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerReader)
#include "test_MsgHandlerReader.moc"
