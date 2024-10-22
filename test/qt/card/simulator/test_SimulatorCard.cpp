/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
 */


#include "SimulatorCard.h"

#include <QtTest>


using namespace governikus;


class test_SimulatorCard
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
		}


		void connection()
		{
			SimulatorCard card((SimulatorFileSystem()));
			QVERIFY(!card.isConnected());

			QCOMPARE(card.establishConnection(), CardReturnCode::OK);
			QVERIFY(card.isConnected());
			QCOMPARE(card.establishConnection(), CardReturnCode::COMMAND_FAILED);
			QVERIFY(card.isConnected());

			QCOMPARE(card.releaseConnection(), CardReturnCode::OK);
			QVERIFY(!card.isConnected());
			QCOMPARE(card.releaseConnection(), CardReturnCode::COMMAND_FAILED);
			QVERIFY(!card.isConnected());
		}


		void transmit_data()
		{
			// Fill mChatPACE for PACE
			const auto& initPace = QByteArray("0022C1A427800A04007F0007020204020283010384010D7F4C12060904007F0007030102025305000513FF00");
			// Fill mCaKeyId for the chip authentication
			const auto& initCa = QByteArray("002241A40F800A04007F00070202030202840129");
			// Fill mRiKeyId for the restricted identification
			const auto& initRi = QByteArray("002241A40F800A04007F00070202050203840101");
			// Fill mAuxiliaryData with the community id for verify
			const auto& initTa = QByteArray("002281A46D800A04007F00070202020203830E444544454D4F5041413030"
											"353239672D7315060904007F000703010402530832303234303330397314"
											"060904007F000703010403530702760503150000912001EAEDF8E53EBD73"
											"58E56B4A6C2BBB8173CE3EAE8C61CD769C57F0210B24FAA0");

			QTest::addColumn<QByteArray>("init");
			QTest::addColumn<QByteArray>("command");
			QTest::addColumn<QString>("response");

			QTest::newRow("select existing") << QByteArray() << QByteArray("00A4020C022F00") << QStringLiteral("9000");
			QTest::newRow("select missing") << QByteArray() << QByteArray("00A4020C022F01") << QStringLiteral("6A80");
			QTest::newRow("readBinary existing") << QByteArray() << QByteArray("00B0810000") << QStringLiteral("6104130249449000");
			QTest::newRow("readBinary missing") << QByteArray() << QByteArray("00B0A10000") << QStringLiteral("6A82");
			QTest::newRow("updatebinary existing") << QByteArray() << QByteArray("00D6810006610413024944") << QStringLiteral("9000");
			QTest::newRow("updatebinary missing") << QByteArray() << QByteArray("00D6A10006610413024944") << QStringLiteral("6A82");
			QTest::newRow("getChallenge") << QByteArray() << QByteArray("0084000008") << QStringLiteral("01020304050607089000");
			QTest::newRow("mseSetAtPace") << QByteArray() << initPace << QStringLiteral("9000");
			QTest::newRow("mseSetAtRi") << QByteArray() << initRi << QStringLiteral("9000");
			QTest::newRow("mseSetAtTa") << QByteArray() << initTa << QStringLiteral("9000");
			QTest::newRow("generalAuthenticatePace")
				<< initPace
				<< QByteArray("10860000027C0000")
				<< QStringLiteral("7C128010[A-F0-9]{32}9000");
			QTest::newRow("generalAuthenticateCa")
				<< initCa
				<< QByteArray("00860000457C4380410493ECE64A5A0CA9DF88F34D274DB546BB7AE657B85D269FBAAD2A471"
						  "95654A4DE136E3A8A92081DD88C578D056EC5607FFA3C53A7B9A652958FDB114757F65B7B00")
				<< QStringLiteral("7C1481080001020304050607820882F3A9399B5779559000");
			QTest::newRow("generalAuthenticateRi")
				<< initRi
				<< QByteArray("008600000001257C820121A082011D060A04007F000702020502038120A9FB57DBA1EEA9BC3E"
						  "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
						  "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
						  "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
						  "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
						  "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
						  "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
						  "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QStringLiteral("7C2281206C877EC6E470BCC1C40F5AAC180D90EDB907AC091E33D402BDD3C99CD0DDE5039000");
			QTest::newRow("generalAuthenticateRi - wrong key")
				<< initRi
				<< QByteArray("008600000001257C820121A082011D060A04007F000702020502018120A9FB57DBA1EEA9BC3E"
						  "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
						  "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
						  "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
						  "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
						  "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
						  "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
						  "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QStringLiteral("6981");
			QTest::newRow("verify - existing")
				<< initTa
				<< QByteArray("802080000B060904007f000703010403")
				<< QStringLiteral("9000");
			QTest::newRow("verify - unknown")
				<< initTa
				<< QByteArray("802080000B060904007f000703010404")
				<< QStringLiteral("6300");
			QTest::newRow("verify - not proprietary")
				<< initTa
				<< QByteArray("002080000B060904007f000703010403")
				<< QStringLiteral("6E00");
		}


		void transmit()
		{
			QFETCH(QByteArray, init);
			QFETCH(QByteArray, command);
			QFETCH(QString, response);
			SimulatorCard card((SimulatorFileSystem()));

			if (!init.isEmpty())
			{
				card.transmit(CommandApdu(QByteArray::fromHex(init)));
			}

			const auto& [returnCode, responseApdu] = card.transmit(CommandApdu(QByteArray::fromHex(command)));
			QCOMPARE(returnCode, CardReturnCode::OK);

			QRegularExpression re(QStringLiteral("^%1$").arg(response.toLower()));
			QVERIFY(re.match(QString::fromLatin1(QByteArray(responseApdu).toHex())).hasMatch());
		}


		void establishPaceChannel_data()
		{
			QTest::addColumn<bool>("isAuthentication");

			QTest::newRow("ChangePin") << false;
			QTest::newRow("Authentication") << true;
		}


		void establishPaceChannel()
		{
			QFETCH(bool, isAuthentication);

			SimulatorFileSystem fileSystem;
			SimulatorCard card(fileSystem);

			EstablishPaceChannelOutput output(CardReturnCode::OK);
			output.setPaceReturnCode(CardReturnCode::OK);
			output.setStatusMseSetAt(QByteArray::fromHex("9000"));
			output.setEfCardAccess(fileSystem.getEfCardAccess());
			if (isAuthentication)
			{
				output.setCarCurr(QByteArray("DETESTeID00005"));
				output.setIdIcc(QByteArray::fromHex("0102030405060708900A0B0C0D0E0F1011121314"));
			}

			const QByteArray data(isAuthentication ? "Chat/CertificateDescription" : "");
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, data, data), output);
		}


		void destroyPaceChannel()
		{
			SimulatorCard card((SimulatorFileSystem()));

			QCOMPARE(card.destroyPaceChannel(), CardReturnCode::OK);
		}


		void setEidPin()
		{
			SimulatorCard card((SimulatorFileSystem()));

			ResponseApduResult result {CardReturnCode::OK, ResponseApdu(StatusCode::SUCCESS)};
			QCOMPARE(card.setEidPin(60), result);
		}


};

QTEST_GUILESS_MAIN(test_SimulatorCard)
#include "test_SimulatorCard.moc"
