/*!
 * \brief Unit tests for \ref MsgHandlerReaderList
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

class test_MsgHandlerReaderList
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


		void noReader()
		{
			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"reader\":[]}"));
		}


		void oneReaderWithoutCard()
		{
			MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"reader\":[{\"attached\":true,\"card\":null,\"keypad\":false,\"name\":\"MockReader 0815\"}]}"));
		}


		void oneReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"reader\":[{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"name\":\"MockReader 0815\"}]}"));
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
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");

			QByteArray expected("{\"msg\":\"READER_LIST\",\"reader\":["
								"{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"name\":\"MockReader 0815\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":false,\"inoperative\":false,\"retryCounter\":-1},\"keypad\":false,\"name\":\"ReaderMock\"},"
								"{\"attached\":true,\"card\":null,\"keypad\":false,\"name\":\"ReaderMockXYZ\"},"
								"{\"attached\":true,\"card\":null,\"keypad\":false,\"name\":\"SpecialMock\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":true,\"inoperative\":false,\"retryCounter\":3},\"keypad\":false,\"name\":\"SpecialMockWithGermanCard\"}"
								"]}");

			QCOMPARE(dispatcher.processCommand(msg), expected);
		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerReaderList)
#include "test_MsgHandlerReaderList.moc"
