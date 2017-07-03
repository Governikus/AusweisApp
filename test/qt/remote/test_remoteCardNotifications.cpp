/*!
 * \brief Unit tests for \ref RemoteCardNotifications
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */
#include "RemoteCardNotifications.h"

#include "RemoteCardNotificationChecker.h"

#include <QtCore/QtCore>
#include <QtTest/QtTest>

using namespace governikus;

class test_remoteCardNotifications
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteCardNotificationChecker> mChecker;

	private Q_SLOTS:
		void remoteReaderDiscoverCmd()
		{
			const RemoteReaderDiscoverCmd notification;

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());
			const QJsonObject& object = document.object();
			QCOMPARE(object.size(), 1);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("REMOTE_READER_DISCOVER"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"REMOTE_READER_DISCOVER\"\n"
							   "}\n"));
		}


		void remoteReaderOfferMsg()
		{
			const RemoteReaderOfferMsg notification(QStringLiteral("Sony Xperia Z5 compact"),
					true,
					24728,
					QVector<int>({1, 2, 3, 4}));

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 5);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("REMOTE_READER_OFFER"));
			QCOMPARE(object.value(QLatin1String("deviceName")).toString(), QStringLiteral("Sony Xperia Z5 compact"));
			QVERIFY(object.value(QLatin1String("encrypted")).toBool());
			QCOMPARE(object.value(QLatin1String("port")).toInt(), 24728);
			const QJsonValue apiLevels = object.value(QLatin1String("availableApiLevels"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 4);
			QCOMPARE(apiLevels.toArray().at(0).toInt(), 1);
			QCOMPARE(apiLevels.toArray().at(1).toInt(), 2);
			QCOMPARE(apiLevels.toArray().at(2).toInt(), 3);
			QCOMPARE(apiLevels.toArray().at(3).toInt(), 4);

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"availableApiLevels\": [\n"
							   "        1,\n"
							   "        2,\n"
							   "        3,\n"
							   "        4\n"
							   "    ],\n"
							   "    \"deviceName\": \"Sony Xperia Z5 compact\",\n"
							   "    \"encrypted\": true,\n"
							   "    \"msg\": \"REMOTE_READER_OFFER\",\n"
							   "    \"port\": 24728\n"
							   "}\n"));
		}


		void getApiLevelCmd()
		{
			const GetApiLevelCmd notification;

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 1);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("GET_API_LEVEL"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"GET_API_LEVEL\"\n"
							   "}\n"));
		}


		void setApiLevelCmd()
		{
			const SetApiLevelCmd notification(1);

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 2);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("SET_API_LEVEL"));
			QCOMPARE(object.value(QLatin1String("level")).toInt(), 1);

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"SET_API_LEVEL\",\n"
							   "    \"level\": 1\n"
							   "}\n"));
		}


		void apiLevelMsg()
		{
			const ApiLevelMsg notification(QStringLiteral("No error"), QVector<int>({1, 2, 3, 4}), 2);

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 4);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("API_LEVEL"));
			QCOMPARE(object.value(QLatin1String("error")).toString(), QStringLiteral("No error"));
			const QJsonValue apiLevels = object.value(QLatin1String("available"));
			QVERIFY(apiLevels.isArray());
			QCOMPARE(apiLevels.toArray().size(), 4);
			QCOMPARE(apiLevels.toArray().at(0).toInt(), 1);
			QCOMPARE(apiLevels.toArray().at(1).toInt(), 2);
			QCOMPARE(apiLevels.toArray().at(2).toInt(), 3);
			QCOMPARE(apiLevels.toArray().at(3).toInt(), 4);

			QCOMPARE(object.value(QLatin1String("current")).toInt(), 2);

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"available\": [\n"
							   "        1,\n"
							   "        2,\n"
							   "        3,\n"
							   "        4\n"
							   "    ],\n"
							   "    \"current\": 2,\n"
							   "    \"error\": \"No error\",\n"
							   "    \"msg\": \"API_LEVEL\"\n"
							   "}\n"));
		}


		void getReaderListCmd()
		{
			GetReaderListCmd notification;

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());
			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 1);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("GET_READER_LIST"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"GET_READER_LIST\"\n"
							   "}\n"));
		}


		void readerListMsg()
		{
			const ReaderListMsg notification(QVector<ReaderDescription>({
						ReaderDescription(QStringLiteral("NFC Reader"),
						true /* attached */,
						ExtendedLengthApduSupportCode::SUPPORTED,
						CardDescription(QByteArray::fromHex("3181C10000"),
						3,
						false /* pinDeactivated  */,
						false /* connected */))
					}));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 2);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("READER_LIST"));

			const QJsonValue jsonReaders = object.value(QLatin1String("readers"));
			QVERIFY(jsonReaders.isArray());
			QCOMPARE(jsonReaders.toArray().size(), 1);

			const QJsonValue jsonReader = jsonReaders.toArray().at(0);
			QVERIFY(jsonReader.isObject());
			const QJsonObject readerObject = jsonReader.toObject();

			QCOMPARE(readerObject.value(QLatin1String("name")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(readerObject.value(QLatin1String("attached")).toBool(), true);
			QCOMPARE(readerObject.value(QLatin1String("extendedLength")).toString(), QStringLiteral("SUPPORTED"));

			const QJsonValue jsonCard = readerObject.value(QLatin1String("card"));
			QVERIFY(jsonCard.isObject());
			const QJsonObject cardObject = jsonCard.toObject();

			QCOMPARE(cardObject.value(QLatin1String("EFCardAccess")).toString().toUpper(), QStringLiteral("3181C10000"));
			QCOMPARE(cardObject.value(QLatin1String("retryCounter")).toInt(), 3);
			QVERIFY(!cardObject.value(QLatin1String("pinDeactivated")).toBool(true));
			QVERIFY(!cardObject.value(QLatin1String("connected")).toBool(true));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"msg\": \"READER_LIST\",\n"
							   "    \"readers\": [\n"
							   "        {\n"
							   "            \"attached\": true,\n"
							   "            \"card\": {\n"
							   "                \"EFCardAccess\": \"3181c10000\",\n"
							   "                \"connected\": false,\n"
							   "                \"pinDeactivated\": false,\n"
							   "                \"retryCounter\": 3\n"
							   "            },\n"
							   "            \"extendedLength\": \"SUPPORTED\",\n"
							   "            \"name\": \"NFC Reader\"\n"
							   "        }\n"
							   "    ]\n"
							   "}\n"));
		}


		void readerMsg()
		{
			const ReaderMsg notification(ReaderDescription(QStringLiteral("NFC Reader"),
					false /* attached */,
					ExtendedLengthApduSupportCode::SUPPORTED,
					CardDescription()),
					QStringLiteral("Optional error message"));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 3);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("READER"));

			const QJsonValue jsonReader = object.value(QLatin1String("reader"));
			QVERIFY(jsonReader.isObject());
			const QJsonObject readerObject = jsonReader.toObject();
			QCOMPARE(readerObject.size(), 4);
			QCOMPARE(readerObject.value(QLatin1String("name")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(readerObject.value(QLatin1String("attached")).toBool(true), false);
			QCOMPARE(readerObject.value(QLatin1String("extendedLength")).toString(), QStringLiteral("SUPPORTED"));

			// No card is represented by null.
			QVERIFY(readerObject.value(QLatin1String("card")).isNull());

			QCOMPARE(object.value(QLatin1String("error")).toString(), QStringLiteral("Optional error message"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"error\": \"Optional error message\",\n"
							   "    \"msg\": \"READER\",\n"
							   "    \"reader\": {\n"
							   "        \"attached\": false,\n"
							   "        \"card\": null,\n"
							   "        \"extendedLength\": \"SUPPORTED\",\n"
							   "        \"name\": \"NFC Reader\"\n"
							   "    }\n"
							   "}\n"));
		}


		void connectCmd()
		{
			const ConnectCmd notification(QStringLiteral("NFC Reader"));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 2);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("CONNECT"));
			QCOMPARE(object.value(QLatin1String("readerName")).toString(), QStringLiteral("NFC Reader"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"CONNECT\",\n"
							   "    \"readerName\": \"NFC Reader\"\n"
							   "}\n"));
		}


		void disconnectCmd()
		{
			const DisconnectCmd notification(QStringLiteral("NFC Reader"));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 2);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("DISCONNECT"));
			QCOMPARE(object.value(QLatin1String("readerName")).toString(), QStringLiteral("NFC Reader"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"DISCONNECT\",\n"
							   "    \"readerName\": \"NFC Reader\"\n"
							   "}\n"));
		}


		void transmitCmd()
		{
			const TransmitCmd notification(QStringLiteral("NFC Reader"), QByteArray::fromHex("00A402022F00"));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 3);
			QCOMPARE(object.value(QLatin1String("cmd")).toString(), QStringLiteral("TRANSMIT"));
			QCOMPARE(object.value(QLatin1String("readerName")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("commandApdu")).toString().toUpper(), QStringLiteral("00A402022F00"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"cmd\": \"TRANSMIT\",\n"
							   "    \"commandApdu\": \"00a402022f00\",\n"
							   "    \"readerName\": \"NFC Reader\"\n"
							   "}\n"));
		}


		void transmitResponseMsg()
		{
			const TransmitResponseMsg notification(QStringLiteral("NFC Reader"),
					QStringLiteral("Optional error message"),
					QByteArray::fromHex("9000"));

			notification.accept(mChecker);

			// Json
			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 4);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("TRANSMIT_RESPONSE"));
			QCOMPARE(object.value(QLatin1String("readerName")).toString(), QStringLiteral("NFC Reader"));
			QCOMPARE(object.value(QLatin1String("error")).toString(), QStringLiteral("Optional error message"));
			QCOMPARE(object.value(QLatin1String("responseApdu")).toString().toUpper(), QStringLiteral("9000"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"error\": \"Optional error message\",\n"
							   "    \"msg\": \"TRANSMIT_RESPONSE\",\n"
							   "    \"readerName\": \"NFC Reader\",\n"
							   "    \"responseApdu\": \"9000\"\n"
							   "}\n"));
		}


		void unsupportedMsg()
		{
			const UnsupportedMsg notification(QStringLiteral("Invalid command"));

			notification.accept(mChecker);

			const QJsonDocument document = notification.toJson();
			QVERIFY(document.isObject());

			const QJsonObject object = document.object();
			QCOMPARE(object.size(), 2);
			QCOMPARE(object.value(QLatin1String("msg")).toString(), QStringLiteral("UNSUPPORTED"));
			QCOMPARE(object.value(QLatin1String("error")).toString(), QStringLiteral("Invalid command"));

			QCOMPARE(document.toJson(),
					QByteArray("{\n"
							   "    \"error\": \"Invalid command\",\n"
							   "    \"msg\": \"UNSUPPORTED\"\n"
							   "}\n"));
		}


	public:
		test_remoteCardNotifications()
			: mChecker(new RemoteCardNotificationChecker())
		{
		}


};

QTEST_GUILESS_MAIN(test_remoteCardNotifications)
#include "test_remoteCardNotifications.moc"
