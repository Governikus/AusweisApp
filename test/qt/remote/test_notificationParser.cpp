/*!
 * \brief Unit tests for \ref NotificationParser
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */
#include "NotificationParser.h"

#include "RemoteCardNotificationChecker.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_notificationParser
	: public QObject
{
	Q_OBJECT

	private:
		const NotificationParser mParser;

		const QSharedPointer<RemoteCardNotificationChecker> mChecker;

		void parseAndVerify(const char* pJsonByteData)
		{
			const QByteArray jsonData(pJsonByteData);
			const QSharedPointer<const RemoteCardNotification> notification = mParser.parse(jsonData);

			QVERIFY(!notification.isNull());
			notification->accept(mChecker);
		}


	private Q_SLOTS:
		void invalidJsonInput()
		{
			const QByteArray jsonData("bla bla");
			const QSharedPointer<const RemoteCardNotification> notification = mParser.parse(jsonData);

			QVERIFY(notification.isNull());
		}


		void remoteReaderDiscoverCmd()
		{
			parseAndVerify("{\n\"cmd\": \"REMOTE_READER_DISCOVER\"\n}");
		}


		void remoteReaderOfferMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"REMOTE_READER_OFFER\",\n"
						   "    \"deviceName\": \"Sony Xperia Z5 compact\",\n"
						   "    \"encrypted\": true,\n"
						   "    \"port\": 24728,\n"
						   "    \"availableApiLevels\": [1, 2, 3, 4]\n"
						   "}");
		}


		void getApiLevelCmd()
		{
			parseAndVerify("{\n\"cmd\": \"GET_API_LEVEL\"\n}");
		}


		void setApiLevelCmd()
		{
			parseAndVerify("{\n"
						   "    \"cmd\": \"SET_API_LEVEL\",\n"
						   "    \"level\": 1\n"
						   "}");
		}


		void apiLevelMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"API_LEVEL\",\n"
						   "    \"error\": \"No error\",\n"
						   "    \"available\": [1, 2, 3, 4],\n"
						   "    \"current\": 2\n"
						   "}");
		}


		void getReaderListCmd()
		{
			parseAndVerify("{\n \"cmd\": \"GET_READER_LIST\" }");
		}


		void readerListMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"READER_LIST\",\n"
						   "    \"readers\": [\n"
						   "        {\n"
						   "            \"name\": \"NFC Reader\",\n"
						   "            \"attached\": true,\n"
						   "            \"extendedLength\": \"SUPPORTED\",\n"
						   "            \"card\": {\n"
						   "                \"EFCardAccess\": \"3181C10000\",\n"
						   "                \"retryCounter\": 3,\n"
						   "                \"pinDeactivated\": false,\n"
						   "                \"connected\": false\n"
						   "            }\n"
						   "        }\n"
						   "    ]\n"
						   "}");
		}


		void readerMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"READER\",\n"
						   "    \"reader\": {\n"
						   "        \"name\": \"NFC Reader\",\n"
						   "        \"attached\": false,\n"
						   "        \"extendedLength\": \"SUPPORTED\",\n"
						   "        \"card\": null\n"
						   "    },\n"
						   "    \"error\": \"Optional error message\"\n"
						   "}");
		}


		void connectCmd()
		{
			parseAndVerify("{\n"
						   "    \"cmd\": \"CONNECT\",\n"
						   "    \"readerName\": \"NFC Reader\"\n"
						   "}");
		}


		void disconnectCmd()
		{
			parseAndVerify("{\n"
						   "    \"cmd\": \"DISCONNECT\",\n"
						   "    \"readerName\": \"NFC Reader\"\n"
						   "}");
		}


		void transmitCmd()
		{
			parseAndVerify("{\n"
						   "    \"cmd\": \"TRANSMIT\",\n"
						   "    \"readerName\": \"NFC Reader\",\n"
						   "    \"commandApdu\": \"00A402022F00\"\n"
						   "}");
		}


		void transmitResponseMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"TRANSMIT_RESPONSE\",\n"
						   "    \"readerName\": \"NFC Reader\",\n"
						   "    \"error\": \"Optional error message\",\n"
						   "    \"responseApdu\": \"9000\"\n"
						   "}");
		}


		void unsupportedMsg()
		{
			parseAndVerify("{\n"
						   "    \"msg\": \"UNSUPPORTED\",\n"
						   "    \"error\": \"Invalid command\"\n"
						   "}");
		}


	public:
		test_notificationParser()
			: mParser()
			, mChecker(new RemoteCardNotificationChecker())
		{
		}


};

QTEST_GUILESS_MAIN(test_notificationParser)
#include "test_notificationParser.moc"
