/*!
 * \brief Unit tests for \ref MsgHandlerReaderList
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MessageDispatcher.h"
#include "MockReaderManagerPlugIn.h"
#include "ReaderManager.h"
#include "messages/MsgHandlerReader.h"

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
			ReaderManager::getInstance().init();
			ReaderManager::getInstance().getPlugInInfos(); // just to wait until initialization finished
		}


		void cleanupTestCase()
		{
			ReaderManager::getInstance().shutdown();
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
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"reader\":[{\"attached\":true,\"card\":null,\"name\":\"MockReader 0815\"}]}"));
		}


		void oneReaderWithCard()
		{
			MockReader* reader = MockReaderManagerPlugIn::getInstance().addReader("MockReader 0815");
			reader->setCard(MockCardConfig());

			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");
			QCOMPARE(dispatcher.processCommand(msg), QByteArray("{\"msg\":\"READER_LIST\",\"reader\":[{\"attached\":true,\"card\":{\"deactivated\":false,\"retryCounter\":-1},\"name\":\"MockReader 0815\"}]}"));
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
			reader->getReaderInfo().setCardInfo(CardInfo(CardType::UNKNOWN));


			reader = MockReaderManagerPlugIn::getInstance().addReader("SpecialMockWithGermanCard");
			reader->setCard(MockCardConfig());
			auto cardInfo = CardInfo(CardType::EID_CARD, QSharedPointer<const EFCardAccess>(), 3, true);
			reader->getReaderInfo().setCardInfo(cardInfo);


			MessageDispatcher dispatcher;
			QByteArray msg("{\"cmd\": \"GET_READER_LIST\"}");

			QByteArray expected("{\"msg\":\"READER_LIST\",\"reader\":["
								"{\"attached\":true,\"card\":{\"deactivated\":false,\"retryCounter\":-1},\"name\":\"MockReader 0815\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":false,\"retryCounter\":-1},\"name\":\"ReaderMock\"},"
								"{\"attached\":true,\"card\":null,\"name\":\"ReaderMockXYZ\"},"
								"{\"attached\":true,\"card\":null,\"name\":\"SpecialMock\"},"
								"{\"attached\":true,\"card\":{\"deactivated\":true,\"retryCounter\":3},\"name\":\"SpecialMockWithGermanCard\"}"
								"]}");

			QCOMPARE(dispatcher.processCommand(msg), expected);

		}


};

QTEST_GUILESS_MAIN(test_MsgHandlerReaderList)
#include "test_MsgHandlerReaderList.moc"
