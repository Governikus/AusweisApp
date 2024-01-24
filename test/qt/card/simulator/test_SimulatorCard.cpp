/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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
			QTest::addColumn<QByteArray>("command");
			QTest::addColumn<QByteArray>("response");

			QTest::newRow("select existing") << QByteArray::fromHex("00A4020C022F00") << QByteArray::fromHex("9000");
			QTest::newRow("select missing") << QByteArray::fromHex("00A4020C022F01") << QByteArray::fromHex("6A80");
			QTest::newRow("readBinary existing") << QByteArray::fromHex("00B0810000") << QByteArray::fromHex("6104130249449000");
			QTest::newRow("readBinary missing") << QByteArray::fromHex("00B0A10000") << QByteArray::fromHex("6A82");
			QTest::newRow("updatebinary existing") << QByteArray::fromHex("00D6810006610413024944") << QByteArray::fromHex("9000");
			QTest::newRow("updatebinary missing") << QByteArray::fromHex("00D6A10006610413024944") << QByteArray::fromHex("6A82");
			QTest::newRow("getChallenge") << QByteArray::fromHex("0084000008") << QByteArray::fromHex("01020304050607089000");
			QTest::newRow("mseSetAtRi") << QByteArray::fromHex("002241A40F800A04007F00070202050203840101") << QByteArray::fromHex("9000");
			QTest::newRow("mseSetAtTa") << QByteArray::fromHex("002281A41867167314060904007f000703010403530702760503150000") << QByteArray::fromHex("9000");
			QTest::newRow("generalAuthenticateCa")
				<< QByteArray::fromHex("00860000457C4380410493ECE64A5A0CA9DF88F34D274DB546BB7AE657B85D269FBAAD2A471"
									   "95654A4DE136E3A8A92081DD88C578D056EC5607FFA3C53A7B9A652958FDB114757F65B7B00")
				<< QByteArray::fromHex("7C1481080001020304050607820882F3A9399B5779559000");
			QTest::newRow("generalAuthenticateRi")
				<< QByteArray::fromHex("008600000001257C820121A082011D060A04007F000702020502038120A9FB57DBA1EEA9BC3E"
									   "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
									   "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
									   "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
									   "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
									   "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
									   "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
									   "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QByteArray::fromHex("7C2281206C877EC6E470BCC1C40F5AAC180D90EDB907AC091E33D402BDD3C99CD0DDE5039000");
			QTest::newRow("generalAuthenticateRi - wrong key")
				<< QByteArray::fromHex("008600000001257C820121A082011D060A04007F000702020502018120A9FB57DBA1EEA9BC3E"
									   "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
									   "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
									   "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
									   "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
									   "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
									   "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
									   "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QByteArray::fromHex("6981");
			QTest::newRow("verify - existing") << QByteArray::fromHex("802080000B060904007f000703010403") << QByteArray::fromHex("9000");
			QTest::newRow("verify - unknown") << QByteArray::fromHex("802080000B060904007f000703010404") << QByteArray::fromHex("6300");
			QTest::newRow("verify - not proprietary") << QByteArray::fromHex("002080000B060904007f000703010403") << QByteArray::fromHex("6E00");
		}


		void transmit()
		{
			QFETCH(QByteArray, command);
			QFETCH(QByteArray, response);
			SimulatorCard card((SimulatorFileSystem()));

			// Fill mCaKeyId for the restricted identification
			card.transmit(CommandApdu(QByteArray::fromHex("002241A40F800A04007F00070202030202840129")));

			// Fill mRiKeyId for the restricted identification
			card.transmit(CommandApdu(QByteArray::fromHex("002241A40F800A04007F00070202050203840101")));

			// Fill mAuxiliaryData with the community id for verify
			card.transmit(CommandApdu(QByteArray::fromHex("002281A41867167314060904007f000703010403530702760503150000")));

			ResponseApduResult result {CardReturnCode::OK, ResponseApdu(response)};
			QCOMPARE(card.transmit(CommandApdu(command)), result);
		}


		void establishPaceChannel()
		{
			SimulatorFileSystem fileSystem;
			SimulatorCard card(fileSystem);

			EstablishPaceChannelOutput output(CardReturnCode::OK);
			output.setPaceReturnCode(CardReturnCode::OK);
			output.setStatusMseSetAt(QByteArray::fromHex("9000"));
			output.setEfCardAccess(fileSystem.getEfCardAccess());
			output.setCarCurr(QByteArray("DETESTeID00005"));
			output.setIdIcc(QByteArray::fromHex("0102030405060708900A0B0C0D0E0F1011121314"));
			QCOMPARE(card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, QByteArray(), QByteArray(), 60), output);
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
