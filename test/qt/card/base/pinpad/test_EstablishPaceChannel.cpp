/*!
 * \brief Tests for card EstablishPaceChannelBuilder
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "pinpad/EstablishPaceChannel.h"


using namespace governikus;


class test_EstablishPaceChannel
	: public QObject
{
	Q_OBJECT

	const QByteArray mChatHex = "7F4C12060904007F00070301020253050000000F0F";
	const QByteArray mCertHex = "30 8202A4"
								"06 0A 04007F00070301030103"
								"A1 0E 0C0C442D547275737420476D6248"
								"A3 3A 0C38476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E"
								"A5 820248"
								"04 820244 4E616D652C20416E7363687269667420756E6420452D4D61696C2D4164726573736520646573204469656E737465616E626965746572733A0D0A476573616D7476657262616E64206465722064657574736368656E20566572736963686572756E67737769727473636861667420652E562E0D0A57696C68656C6D73747261C39F652034332F3433670D0A3130313137204265726C696E0D0A6265726C696E406764762E64650D0A0D0A4765736368C3A46674737A7765636B3A0D0A2D52656769737472696572756E6720756E64204C6F67696E20616D204744562D4D616B6C6572706F7274616C2D0D0A0D0A48696E7765697320617566206469652066C3BC722064656E204469656E737465616E626965746572207A757374C3A46E646967656E205374656C6C656E2C20646965206469652045696E68616C74756E672064657220566F7273636872696674656E207A756D20446174656E73636875747A206B6F6E74726F6C6C696572656E3A0D0A4265726C696E6572204265617566747261677465722066C3BC7220446174656E73636875747A20756E6420496E666F726D6174696F6E7366726569686569740D0A416E20646572205572616E696120342D31300D0A3130373837204265726C696E0D0A3033302F3133382038392D300D0A6D61696C626F7840646174656E73636875747A2D6265726C696E2E64650D0A687474703A2F2F7777772E646174656E73636875747A2D6265726C696E2E64650D0A416E737072656368706172746E65723A2044722E20416C6578616E64657220446978";

	private Q_SLOTS:
		void setPasswordId()
		{
			QByteArray hexBytes("02 05 00"
								"03"
								"00"
								"00"
								"00 00");

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN);

			QCOMPARE(builder.createCommandData(), QByteArray::fromHex(hexBytes));
		}


		void setPasswordIdCcid()
		{
			QByteArray hexBytes("FF 9A 04 02 07"
								"30 05"
								"        A1 03 02 01 03"
								"00");

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN);

			QCOMPARE(builder.createCommandDataCcid(), QByteArray::fromHex(hexBytes));
		}


		void setPassword()
		{
			QByteArray hexBytes("02 0B 00"
								"03"
								"00"
								"06 31 32 33 34 35 36"
								"00 00");

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN);
			builder.setPassword(QByteArray("123456"));

			QCOMPARE(builder.createCommandData(), QByteArray::fromHex(hexBytes));
		}


		void setPasswordCcid()
		{
			QByteArray hexBytes("FF 9A 04 02 11"
								"30 0F"
								"        A1 03 02 01 03"
								"        A2 08 12 06 31 32 33 34 35 36"
								"00");

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN);
			builder.setPassword(QByteArray("123456"));

			QCOMPARE(builder.createCommandDataCcid().toHex(), QByteArray::fromHex(hexBytes).toHex());
		}


		void setChat()
		{
			QByteArray hexBytes("02 1A 00"
								"03"
								"15 CHAT"
								"00"
								"00 00");
			hexBytes.replace("CHAT", mChatHex);

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN, QByteArray::fromHex(mChatHex));

			QCOMPARE(builder.getChat(), QByteArray::fromHex(mChatHex));
			QCOMPARE(builder.createCommandData(), QByteArray::fromHex(hexBytes));
		}


		void setChatCcid()
		{
			QByteArray hexBytes("FF 9A 04 02 20"
								"30 1E"
								"        A1 03 02 01 03"
								"        A3 17 04 15 CHAT"
								"00");
			hexBytes.replace("CHAT", mChatHex);

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN, QByteArray::fromHex(mChatHex));

			QCOMPARE(builder.getChat(), QByteArray::fromHex(mChatHex));
			QCOMPARE(builder.createCommandDataCcid(), QByteArray::fromHex(hexBytes));
		}


		void setCertificateDescription()
		{
			QByteArray hexBytes("02 C2 02"
								"03"
								"15 CHAT"
								"00"
								"A8 02 CERT");
			hexBytes.replace("CHAT", mChatHex);
			hexBytes.replace("CERT", mCertHex);

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN, QByteArray::fromHex(mChatHex), QByteArray::fromHex(mCertHex));

			QCOMPARE(builder.getChat(), QByteArray::fromHex(mChatHex));
			QCOMPARE(builder.getCertificateDescription(), QByteArray::fromHex(mCertHex));
			QCOMPARE(builder.createCommandData(), QByteArray::fromHex(hexBytes));
		}


		void setCertificateDescriptionCcid()
		{
			QByteArray hexBytes("FF 9A 04 02 00 02 CE"
								"30 8202CA"
								"        A1 03 02 01 03"
								"        A3 17 04 15 CHAT"
								"        A4 8202A8 CERT"
								"01 00");
			hexBytes.replace("CHAT", mChatHex);
			hexBytes.replace("CERT", mCertHex);

			EstablishPaceChannel builder(PacePasswordId::PACE_PIN, QByteArray::fromHex(mChatHex), QByteArray::fromHex(mCertHex));

			QCOMPARE(builder.getChat(), QByteArray::fromHex(mChatHex));
			QCOMPARE(builder.getCertificateDescription(), QByteArray::fromHex(mCertHex));
			QCOMPARE(builder.createCommandDataCcid(), QByteArray::fromHex(hexBytes));
		}


		void buildAndParse()
		{
			const PacePasswordId pinId(PacePasswordId::PACE_PIN);
			const QByteArray chat = QByteArray::fromHex(mChatHex);
			const QByteArray certDescription = QByteArray::fromHex(mCertHex);

			EstablishPaceChannel builder(pinId, chat, certDescription);
			const auto buffer = builder.createInputData();

			EstablishPaceChannel parser;
			QVERIFY(parser.fromInputData(buffer));
			QCOMPARE(pinId, parser.getPasswordId());
			QCOMPARE(chat, parser.getChat());
			QCOMPARE(certDescription, parser.getCertificateDescription());
		}


		void buildAndParseCcid()
		{
			const PacePasswordId pinId(PacePasswordId::PACE_PIN);
			const QByteArray chat = QByteArray::fromHex(mChatHex);
			const QByteArray certDescription = QByteArray::fromHex(mCertHex);

			EstablishPaceChannel builder(pinId, chat, certDescription);
			const auto buffer = builder.createCommandDataCcid();

			EstablishPaceChannel parser;
			QVERIFY(parser.fromCcid(buffer));
			QCOMPARE(pinId, parser.getPasswordId());
			QCOMPARE(chat, parser.getChat());
			QCOMPARE(certDescription, parser.getCertificateDescription());
		}


};

QTEST_GUILESS_MAIN(test_EstablishPaceChannel)
#include "test_EstablishPaceChannel.moc"
