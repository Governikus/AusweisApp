/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
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
			// Fill mChatPACE for PACE and mTaCertificate for the certificate chain validation
			const QByteArray initPace("0022C1A427800A04007F0007020204020283010384010D7F4C12060904007F0007030102025305000513FF00");
			// Fill mCaKeyId for the chip authentication
			const QByteArray initCa("002241A40F800A04007F00070202030202840129");
			// Fill mRiKeyId for the restricted identification
			const QByteArray initRi("002241A40F800A04007F00070202050203840101");
			// Fill mAuxiliaryData with the community id for verify
			const QByteArray initTa_DETESTeID00005(
				"002281A484800A04007F00070202020203830E4445544553546549443030"
				"30303567447315060904007F000703010401530832303036303631367315"
				"060904007F000703010402530832303234303631367314060904007F0007"
				"03010403530702760503150000912067B3DDB150DBFCDCB8703AFC211E28"
				"DA0FF02083D45C7BC8DBB68E8C9486DDC9");
			const QByteArray initTa_DEDEMOPAA00529(
				"002281A484800A04007F00070202020203830E444544454D4F5041413030"
				"35323967447315060904007F000703010401530832303036303631367315"
				"060904007F000703010402530832303234303631367314060904007F0007"
				"03010403530702760503150000912047F2FD1267DE9D8753EFE8CAF57AB2"
				"95BFEFD8C18B006D3F4751AE7E8301EBF2");

			const QByteArray psoVerify_DETESTeID00006(
				"002a00be0001b67f4e82016e5f290100420e44455445535465494430303030357f4982011d060a04007f000702020202"
				"038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef675"
				"30417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6b"
				"ccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3da"
				"c4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3"
				"b561a6f7901e0e82974856a786410425ab80f9c7bca0ab1759d8e469f911cc006d02131552aa5f248b2a38d7c72cfb33"
				"17ea6881fd24d8b31a2e75fbeda87964b60787095f75c753cd8bc5264d3c9a8701015f200e4445544553546549443030"
				"3030367f4c12060904007f0007030102025305fc0f13ffff5f25060108000200055f24060201000200055f37402e5592"
				"3ed687cb104d609dd183402e8292db03c3effe5ef3fac597d2a8db27370269eaad7341d72447c9184cd817ae0e2bd4df"
				"6fcf89dc52f455d490f077e5e9");
			const QByteArray psoVerify_DETESTeID00007(
				"002a00be0001b67f4e82016e5f290100420e44455445535465494430303030367f4982011d060a04007f000702020202"
				"038120a9fb57dba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef675"
				"30417affe7fb8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6b"
				"ccdc18ff8c07b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3da"
				"c4fd97f8461a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3"
				"b561a6f7901e0e82974856a786410431d8f49a3095d324e52833e1354860fcd797f44730aa4b67486e10e6059a04b773"
				"e16f803a115788d307a7b99296d5ab5cbd658d3ea28d4771ed5a027db5ade28701015f200e4445544553546549443030"
				"3030377f4c12060904007f0007030102025305fc0f13ffff5f25060200010001035f24060203010001035f3740275bad"
				"7ef24614f99abe983c6643be5385d2c2b1d146dd481ac422b1605ca5a64f87d4b2b9f56beee34b8e6b6af6f3423a21f0"
				"0aabf55f29c3771b06b22b3a0a");

			QTest::addColumn<QByteArrayList>("init");
			QTest::addColumn<QByteArray>("command");
			QTest::addColumn<QString>("response");
			QTest::addColumn<QByteArray>("logging");

			QTest::newRow("select existing") << QByteArrayList() << QByteArray("00A4020C022F00") << QStringLiteral("9000") << QByteArray();
			QTest::newRow("select missing") << QByteArrayList() << QByteArray("00A4020C022F01") << QStringLiteral("6A80") << QByteArray();
			QTest::newRow("readBinary existing") << QByteArrayList() << QByteArray("00B0810000") << QStringLiteral("6104130249449000") << QByteArray();
			QTest::newRow("readBinary missing") << QByteArrayList() << QByteArray("00B0A10000") << QStringLiteral("6A82") << QByteArray();
			QTest::newRow("updatebinary existing") << QByteArrayList() << QByteArray("00D6810006610413024944") << QStringLiteral("9000") << QByteArray();
			QTest::newRow("updatebinary missing") << QByteArrayList() << QByteArray("00D6A10006610413024944") << QStringLiteral("6A82") << QByteArray();

			QTest::newRow("getChallenge") << QByteArrayList() << QByteArray("0084000008") << QStringLiteral("[A-F0-9]{16}9000") << QByteArray();
			QTest::newRow("mseSetAtPace") << QByteArrayList() << initPace << QStringLiteral("9000") << QByteArray();
			QTest::newRow("mseSetAtRi") << QByteArrayList() << initRi << QStringLiteral("9000") << QByteArray();
			QTest::newRow("mseSetDst - success") << QByteArrayList({initPace}) << QByteArray("002281b610830e4445544553546549443030303035") << QStringLiteral("9000") << QByteArray();
			QTest::newRow("mseSetDst - fail") << QByteArrayList({initPace}) << QByteArray("002281b610830e4445544553546549443030303036") << QStringLiteral("6A88") << QByteArray();
			QTest::newRow("psoVerify - success") << QByteArrayList({initPace}) << psoVerify_DETESTeID00006 << QStringLiteral("9000") << QByteArray();
			QTest::newRow("psoVerify - fail") << QByteArrayList({initPace}) << psoVerify_DETESTeID00007 << QStringLiteral("6300") << QByteArray();
			QTest::newRow("mseSetAtTa - success") << QByteArrayList({initPace}) << initTa_DETESTeID00005 << QStringLiteral("9000") << QByteArray();
			QTest::newRow("mseSetAtTa - fail") << QByteArrayList({initPace}) << initTa_DEDEMOPAA00529 << QStringLiteral("6A88") << QByteArray();
			QTest::newRow("generalAuthenticatePace")
				<< QByteArrayList({initPace})
				<< QByteArray("10860000027C0000")
				<< QStringLiteral("7C128010[A-F0-9]{32}9000")
				<< QByteArray();
			QTest::newRow("generalAuthenticateCa")
				<< QByteArrayList({initCa})
				<< QByteArray("00860000457C4380410493ECE64A5A0CA9DF88F34D274DB546BB7AE657B85D269FBAAD2A471"
						  "95654A4DE136E3A8A92081DD88C578D056EC5607FFA3C53A7B9A652958FDB114757F65B7B00")
				<< QStringLiteral("7C1481080001020304050607820882F3A9399B5779559000")
				<< QByteArray();
			QTest::newRow("generalAuthenticateRi")
				<< QByteArrayList({initRi})
				<< QByteArray("008600000001257C820121A082011D060A04007F000702020502038120A9FB57DBA1EEA9BC3E"
						  "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
						  "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
						  "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
						  "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
						  "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
						  "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
						  "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QStringLiteral("7C2281206C877EC6E470BCC1C40F5AAC180D90EDB907AC091E33D402BDD3C99CD0DDE5039000")
				<< QByteArray();
			QTest::newRow("generalAuthenticateRi - wrong key")
				<< QByteArrayList({initRi})
				<< QByteArray("008600000001257C820121A082011D060A04007F000702020502018120A9FB57DBA1EEA9BC3E"
						  "660A909D838D726E3BF623D52620282013481D1F6E537782207D5A0975FC2C3057EEF6753041"
						  "7AFFE7FB8055C126DC5C6CE94A4B44F330B5D9832026DC5C6CE94A4B44F330B5D9BBD77CBF95"
						  "8416295CF7E1CE6BCCDC18FF8C07B68441048BD2AEB9CB7E57CB2C4B482FFC81B7AFB9DE27E1"
						  "E3BD23C23A4453BD9ACE3262547EF835C3DAC4FD97F8461A14611DC9C27745132DED8E545C1D"
						  "54C72F0469978520A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E829748"
						  "56A7864104373CD3BAD6245F6EDEB382E066C57107B84BF5EF2834201AAEA41CC0D472890308"
						  "3687C8C37C1D005C61BE2C122364101C2CBC41FFC3EC48C96E3F1D4F646ABE8701010100")
				<< QStringLiteral("6981")
				<< QByteArray();
			QTest::newRow("externalAuthenticate - fail")
				<< QByteArrayList({initPace, initTa_DETESTeID00005})
				<< QByteArray("00820000020102")
				<< QStringLiteral("6300")
				<< QByteArray();
			QTest::newRow("verify - existing")
				<< QByteArrayList({initPace, initTa_DETESTeID00005})
				<< QByteArray("802080000B060904007f000703010403")
				<< QStringLiteral("9000")
				<< QByteArray();
			QTest::newRow("verify - unknown")
				<< QByteArrayList({initPace, initTa_DETESTeID00005})
				<< QByteArray("802080000B060904007f000703010404")
				<< QStringLiteral("6300")
				<< QByteArray();
			QTest::newRow("verify - not proprietary")
				<< QByteArrayList({initPace, initTa_DETESTeID00005})
				<< QByteArray("002080000B060904007f000703010403")
				<< QStringLiteral("6E00")
				<< QByteArray();

			QTest::newRow("activatePin") << QByteArrayList() << QByteArray("00441003") << QStringLiteral("9000") << QByteArray("ACTIVATE PIN");
			QTest::newRow("activatePin - fail 1") << QByteArrayList() << QByteArray("00440003") << QStringLiteral("6A86") << QByteArray();
			QTest::newRow("activatePin - fail 2") << QByteArrayList() << QByteArray("00441002") << QStringLiteral("6A86") << QByteArray();
			QTest::newRow("deactivatePin") << QByteArrayList() << QByteArray("00041003") << QStringLiteral("9000") << QByteArray("DEACTIVATE PIN");
			QTest::newRow("deactivatePin - fail 1") << QByteArrayList() << QByteArray("00040003") << QStringLiteral("6A86") << QByteArray();
			QTest::newRow("deactivatePin - fail 2") << QByteArrayList() << QByteArray("00041002") << QStringLiteral("6A86") << QByteArray();
			QTest::newRow("resetRetryCounter - change") << QByteArrayList() << QByteArray("002C020306313233343536") << QStringLiteral("9000") << QByteArray("CHANGE PIN: \"123456\"");
			QTest::newRow("resetRetryCounter - unblock") << QByteArrayList() << QByteArray("002C0303") << QStringLiteral("9000") << QByteArray("UNBLOCK PIN");
			QTest::newRow("resetRetryCounter - fail 1") << QByteArrayList() << QByteArray("002C1003") << QStringLiteral("6A86") << QByteArray();
			QTest::newRow("resetRetryCounter - fail 2") << QByteArrayList() << QByteArray("002C0002") << QStringLiteral("6A86") << QByteArray();
		}


		void transmit()
		{
			QFETCH(QByteArrayList, init);
			QFETCH(QByteArray, command);
			QFETCH(QString, response);
			QFETCH(QByteArray, logging);
			SimulatorCard card((SimulatorFileSystem()));

			for (const auto& initCmd : init)
			{
				card.transmit(CommandApdu(QByteArray::fromHex(initCmd)));
			}

			if (!logging.isNull())
			{
				QTest::ignoreMessage(QtDebugMsg, logging.data());
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

			const QByteArray chat = QByteArray::fromHex(isAuthentication ? "7f4c12060904007f00070301020253050000000000" : "");
			const QByteArray certificateDescription(isAuthentication ? "CertificateDescription" : "");
			const auto& output = card.establishPaceChannel(PacePasswordId::PACE_PIN, 6, chat, certificateDescription);

			QCOMPARE(output.getPaceReturnCode(), CardReturnCode::OK);
			QCOMPARE(output.getStatusCodeMseSetAt(), StatusCode::SUCCESS);
			QCOMPARE(output.getStatusMseSetAt(), QByteArray::fromHex("9000"));
			QCOMPARE(output.getEfCardAccess(), fileSystem.getEfCardAccess());
			QCOMPARE(output.getCarCurr(), QByteArray(isAuthentication ? "DETESTeID00005" : ""));
			QCOMPARE(output.getCarPrev(), QByteArray());
			QCOMPARE(output.getIdIcc().isEmpty(), !isAuthentication);
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
