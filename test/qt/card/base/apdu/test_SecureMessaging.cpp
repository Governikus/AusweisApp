/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "pace/SecureMessaging.h"

#include "LogHandler.h"
#include "SecurityProtocol.h"
#include "apdu/SecureMessagingCommand.h"
#include "asn1/ASN1Util.h"

#include <QtCore>
#include <QtTest>


using namespace governikus;


/*
 * Test data generated with ocard
 */
class ByteArrayUtil
{
	public:
		template<typename T> static QByteArray fromValue(T pValue)
		{
			return QByteArray::fromHex(QByteArray::number(pValue, 16));
		}


};

class test_SecureMessaging
	: public QObject
{
	Q_OBJECT

	private:
		QScopedPointer<SecureMessaging> mSecureMessagingTerminal;
		QScopedPointer<SecureMessaging> mSecureMessagingCard;
		QScopedPointer<CipherMac> mCipherMac;
		QScopedPointer<SymmetricCipher> mCipher;


		QList<QByteArray> encryptResponse(const QByteArray& pBuffer, quint32& pSsc) const
		{
			QList<QByteArray> result(4);

			++pSsc;
			QByteArray ssc = QByteArray(mCipher->getBlockSize() - QByteArray::number(pSsc, 16).size(), 0x00).append(ByteArrayUtil::fromValue(pSsc));

			if (pBuffer.length() > 2)
			{
				QByteArray pData = pBuffer.left(pBuffer.length() - 2);
				mCipher->setIv(QByteArray().fill(0x00, mCipher->getBlockSize()));
				auto iv = mCipher->encrypt(ssc);
				mCipher->setIv(iv);

				const auto padLen = mCipher->getBlockSize() - (pData.size() % mCipher->getBlockSize());
				pData += QByteArray::fromHex("80") + QByteArray(padLen - 1, 0x00);
				QByteArray encryptedData = mCipher->encrypt(pData).prepend(0x01);
				result[0] = QByteArray().append(static_cast<char>(0x87)).append(ByteArrayUtil::fromValue(encryptedData.size())).append(encryptedData);
			}
			QByteArray pSw = pBuffer.right(2);
			result[1] = QByteArray().append(static_cast<char>(0x99)).append(ByteArrayUtil::fromValue(pSw.size())).append(pSw);
			{
				QByteArray dataToMac = ssc + result[0] + result[1];
				const auto padLen = mCipher->getBlockSize() - (dataToMac.size() % mCipher->getBlockSize());
				dataToMac.append(static_cast<char>(0x80)).append(QByteArray(padLen - 1, 0x00));
				QByteArray mac = mCipherMac->generate(dataToMac);
				result[2] = QByteArray().append(static_cast<char>(0x8E)).append(ByteArrayUtil::fromValue(mac.size())).append(mac);
			}
			result[3] = pSw;

			return result;
		}

	private Q_SLOTS:
		void init()
		{
			SecurityProtocol securityProtocol(KnownOid::ID_PACE_ECDH_GM_AES_CBC_CMAC_128);
			QByteArray encKey("F1234567890ABCDE");
			QByteArray macKey("1234567890ABCDEF");
			mSecureMessagingTerminal.reset(new SecureMessaging(securityProtocol, encKey, macKey));
			mSecureMessagingCard.reset(new SecureMessaging(securityProtocol, encKey, macKey));
			mCipherMac.reset(new CipherMac(securityProtocol, macKey));
			mCipher.reset(new SymmetricCipher(securityProtocol, encKey));
		}


		void testNotInitialized()
		{
			const QByteArray empty;
			QByteArray data;
			const CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::NO_LE);
			SecureMessaging n(SecurityProtocol(Oid(nullptr)), empty, empty);
			CommandApdu commandApdu = n.encrypt(command);
			QVERIFY(commandApdu.isEmpty());
		}


		void testCommandChainingCLA()
		{
			QByteArray data;
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::NO_LE);
			command.enableCommandChaining();
			QCOMPARE(command, QByteArray::fromHex("10222030"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand, QByteArray::fromHex("1C2220300A8E081211BCC1702D612B00"));
		}


		void testNoDataNoLe()
		{
			QByteArray data;
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::NO_LE);
			QCOMPARE(command, QByteArray::fromHex("00222030"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand, QByteArray::fromHex("0C2220300A8E086733A03D8E9AC66900"));
		}


		void testNoDataShortLe()
		{
			QByteArray data;
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, 8);
			QCOMPARE(command, QByteArray::fromHex("0022203008"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand, QByteArray::fromHex("0C2220300D9701088E0844B066BC91C77C4500"));
		}


		void testNoDataShortMaxLe()
		{
			QByteArray data;
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::SHORT_MAX_LE);
			QCOMPARE(command, QByteArray::fromHex("0022203000"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand, QByteArray::fromHex("0C2220300D9701008E0875F18DD4F6B913D500"));
		}


		void testNoDataExtendedMaxLe()
		{
			QByteArray data;
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command, QByteArray::fromHex("00222030000000"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand, QByteArray::fromHex("0C22203000000E970200008E08E5A6AE57AD293C450000"));
		}


		void testDataShortMaxLe()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::SHORT_MAX_LE);
			QCOMPARE(command, QByteArray::fromHex("0022203004D0D1D2D300"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand,
					QByteArray::fromHex("0C22203020871101CC30C03DBE01213486B74CE1F48F5C499701008E08A822685D1780FADD00"));
		}


		void testDataExtendedMaxLe()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command, QByteArray::fromHex("00222030000004D0D1D2D30000"));
			CommandApdu securedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand,
					QByteArray::fromHex("0C222030000021871101CC30C03DBE01213486B74CE1F48F5C49970200008E08C076CEC6A1CC26020000"));
		}


		void testExtendedLengthAfterEcryption()
		{
			const QByteArray data(250, 0x42);
			const CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, 24);
			QCOMPARE(command, QByteArray::fromHex("00222030 fa") + data + QByteArray::fromHex("18"));

			const auto& encryptedCommand = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(encryptedCommand,
					QByteArray::fromHex("0c222030 000112"
										"87820101 01"
										"257c4674cfae012dd0da1d7ae3901cb0acd1789c30d31d6934092bc9db9f01dd7b7d17215a69f02fe4c24e7ad4019113"
										"05984da2c4ceca18f41e0e1f55b4bc5d69fa55d73e9eb5c018b6f9edcb07cf171973d5d8612ffc20326e997083592d31"
										"c0934f2c28d466535d6f4a024cf7068aa44daad8501a228b5dcba0766c6771b5761085d3f19801f92875b10cf17175f7"
										"ec1714e7a9a99714386e518059360910e6d3a6f54111e5aa983f5cdf2d5f5597fc0f1387ab70a06b922fa45cc1d42bbe"
										"9f831235d88edd78a14ca9d9abcd02d799161ddc8c231078303d1062b875eaa899a44824ed25e4673706f9d0063872fd"
										"6dabca6f1fb03e4d4c1a68dd4c6d7488"
										"970118"
										"8e08543fdc3e1ff32dfb"
										"0000"));

			const auto& decryptedcommand = mSecureMessagingCard->decrypt(encryptedCommand);
			QCOMPARE(decryptedcommand, command);
		}


		void testSendSequenceCounter()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(Ins::MSE_SET, 0x20, 0x30, data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command, QByteArray::fromHex("00222030000004D0D1D2D30000"));

			CommandApdu securedCommand1 = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand1,
					QByteArray::fromHex("0C222030000021871101CC30C03DBE01213486B74CE1F48F5C49970200008E08C076CEC6A1CC26020000"));

			CommandApdu securedCommand2 = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand2,
					QByteArray::fromHex("0C22203000002187110140EADE1A656A226767536A7CEC2AC6F9970200008E08451D5C632AB4EE240000"));

			CommandApdu securedCommand3 = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand3,
					QByteArray::fromHex("0C2220300000218711010BDF2B6181A9091E6FE0147D8B11069D970200008E08DEB87800B60D6D110000"));

			CommandApdu securedCommand4 = mSecureMessagingTerminal->encrypt(command);
			QCOMPARE(securedCommand4,
					QByteArray::fromHex("0C222030000021871101252538E3EBAD320FCD01905459F6B6E0970200008E080F1C93C6D9BDBCF20000"));
		}


		void testLastIndexOf()
		{
			// Check if lastIndexOf will return the correct result
			// https://bugreports.qt.io/browse/QTBUG-44223

			const char ISO_LEADING_PAD_BYTE = static_cast<char>(0x80);
			QByteArray data = QByteArray::fromHex("109205A006092B864886FAAD010702A08255913082058D020103310F300D0609608648016503040201050030820148060804007F0007030201A082013A0482013631820132300D060804007F00070202020201023012060A04007F000702020302020201020201413012060A04007F0007020204020202010202010D3017060A04007F0007020205020330090201010201430101FF3017060A04007F0007020205020330090201010201440101003019060904007F000702020502300C060704007F0007010202010D301C060904007F000702020302300C060704007F0007010202010D020141302A060804007F0007020206161E687474703A2F2F62736980");
			QCOMPARE(data.lastIndexOf(ISO_LEADING_PAD_BYTE), 255);
		}


		void testDecrypt_onlyStatusWord()
		{
			QByteArray plainBuffer = QByteArray::fromHex("9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + result[1] + result[2] + result[3]);

			ResponseApdu decryptedResponse = mSecureMessagingTerminal->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse, plainBuffer);
		}


		void testDecrypt()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + result[1] + result[2] + result[3]);

			ResponseApdu decryptedResponse = mSecureMessagingTerminal->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse, plainBuffer);
		}


		void testDecrypt_updateSendSequenceCounter()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");
			QByteArray plainBuffer2 = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + result[1] + result[2] + result[3]);

			QCOMPARE(ssc, 1u);
			ResponseApdu decryptedResponse = mSecureMessagingTerminal->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse, plainBuffer);

			auto result2 = encryptResponse(plainBuffer2, ssc);
			ResponseApdu encryptedResponse2(result2[0] + result2[1] + result2[2] + result2[3]);

			QCOMPARE(ssc, 2u);
			ResponseApdu decryptedResponse2 = mSecureMessagingTerminal->decrypt(encryptedResponse2);
			QCOMPARE(decryptedResponse2, plainBuffer2);
		}


		void testDecrypt_WrongSw()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + result[1] + result[2] + QByteArray::fromHex("6B00"));

			QVERIFY(mSecureMessagingTerminal->decrypt(encryptedResponse).isEmpty());
		}


		void testDecrypt_WrongSecuredSw()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + QByteArray::fromHex("99026B00") + result[2] + result[3]);

			QVERIFY(mSecureMessagingTerminal->decrypt(encryptedResponse).isEmpty());
		}


		void testDecrypt_WrongMac()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(result[0] + result[1] + QByteArray::fromHex("8E080102030404030201") + result[3]);

			QVERIFY(mSecureMessagingTerminal->decrypt(encryptedResponse).isEmpty());
		}


		void testMaximumLengthExeeded()
		{
			CommandApdu apdu = CommandApdu(QByteArray::fromHex("00010203"), QByteArray(65535, 0x42), 1);
			QTest::ignoreMessage(QtCriticalMsg, "Command data exceeds maximum of 0xffff");
			QCOMPARE(mSecureMessagingTerminal->encrypt(apdu), QByteArray());
		}


		void testMessageStructure_data()
		{
			QTest::addColumn<int>("lc");
			QTest::addColumn<int>("le");

			// Cases according to ISO-7816-4 (First edition, 1995-09), 5.3.2 Decoding conventions for command bodies

			QTest::newRow("1") << 0 << 0;

			QTest::newRow("2S 0   1") << 0 << 1;
			QTest::newRow("2S 0 255") << 0 << 255;
			QTest::newRow("2S 0 256") << 0 << 256;

			QTest::newRow("3S   1 0") << 1 << 0;
			QTest::newRow("3S 255 0") << 255 << 0;

			QTest::newRow("4S   1   1 ") << 1 << 1;
			QTest::newRow("4S   1 255") << 1 << 255;
			QTest::newRow("4S   1 256") << 1 << 256;
			QTest::newRow("4S 255   1") << 255 << 1;
			QTest::newRow("4S 255 255") << 255 << 255;
			QTest::newRow("4S 255 256") << 255 << 256;

			QTest::newRow("2E 0   257") << 0 << 257;
			QTest::newRow("2E 0 65535") << 0 << 65535;
			QTest::newRow("2E 0 65536") << 0 << 65536;

			QTest::newRow("3E   256 0") << 256 << 0;
			QTest::newRow("3E 65519 0") << 65519 << 0;

			QTest::newRow("4E   255   257") << 255 << 257;
			QTest::newRow("4E   255 65535") << 255 << 65535;
			QTest::newRow("4E   255 65536") << 255 << 65536;
			QTest::newRow("4E   256     1") << 256 << 1;
			QTest::newRow("4E   256   255") << 256 << 255;
			QTest::newRow("4E   256   256") << 256 << 256;
			QTest::newRow("4E   256   257") << 256 << 257;
			QTest::newRow("4E   256 65535") << 256 << 65535;
			QTest::newRow("4E   256 65536") << 256 << 65536;
			QTest::newRow("4E 65503   257") << 65503 << 257;
			QTest::newRow("4E 65503 65535") << 65503 << 65535;
			QTest::newRow("4E 65503 65536") << 65503 << 65536;
		}


		void testMessageStructure()
		{
			QFETCH(int, lc);
			QFETCH(int, le);

			Env::getSingleton<LogHandler>()->init();
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			const QByteArray data(lc, 0x42);
			const CommandApdu apdu(QByteArray::fromHex("00010203"), data, le);
			const CommandApdu encryptedApdu = mSecureMessagingTerminal->encrypt(apdu);

			SecureMessagingCommand smCmd(encryptedApdu);
			QCOMPARE(Asn1OctetStringUtil::getValue(smCmd.mExpectedLength.data()).size(), le > 0 ? CommandApdu::isExtendedLength(data, le) ? 2 : 1 : 0);

			QCOMPARE(mSecureMessagingCard->decrypt(encryptedApdu), apdu);

			QCOMPARE(logSpy.count(), 0);
			Env::getSingleton<LogHandler>()->reset();
		}


		void fullAuthentication_data()
		{
			QTest::addColumn<QByteArray>("command");
			QTest::addColumn<QByteArray>("response");

			QTest::newRow("01") << QByteArray("00a4000c023f00") << QByteArray("9000");
			QTest::newRow("02") << QByteArray("00a4020c022f00") << QByteArray("9000");
			QTest::newRow("03") << QByteArray("00b000005a") << QByteArray("61324f0fe828bd080fa000000167455349474e500f434941207a752044462e655369676e5100730c4f0aa000000167455349474e61094f07a0000002471001610b4f09e80704007f00070302610c4f0aa000000167455349474e9000");
			QTest::newRow("04") << QByteArray("00a4020c02011c") << QByteArray("9000");
			QTest::newRow("05") << QByteArray("00b00000ff") << QByteArray("3181c1300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273692e62756e642e64652f6369662f6e70612e786d6c303e060804007f000702020831323012060a04007f00070202030202020102020145301c060904007f000702020302300c060704007f0007010202010d020145006282");
			QTest::newRow("06") << QByteArray("0022c1a412800a04007f0007020204020283010384010d") << QByteArray("63c2");
			QTest::newRow("07") << QByteArray("0022c1a412800a04007f0007020204020283010384010d") << QByteArray("63c2");
			QTest::newRow("08") << QByteArray("0084000008") << QByteArray("791ef124c77d49089000");
			QTest::newRow("09") << QByteArray("002281b610830e4445544553546549443030303037") << QByteArray("9000");
			QTest::newRow("10") << QByteArray("002a00bee77f4e81a05f290100420e44455445535465494430303030377f494f060a04007f0007020202020386410427fec2155a2ef22d8ca7ced69f72e628d576724754ed4ba566183e62dfed93e88a3c8ef4e6a268cbc11bc740e0465f3151c39fe28dca36445764cbb97161271d5f20104445445674494447564b3330303031327f4c12060904007f0007030102025305400513ff975f25060201000202085f24060201000502095f374079a595788bd042d42eb0e8e16659e7c829a1847a4cfe15d1d43a55f3009b7b1f46985d45dac8781d9ca5fe509fa31f824e332b7418c3577ac74cc0d28b794d19") << QByteArray("9000");
			QTest::newRow("11") << QByteArray("002281b61283104445445674494447564b333030303132") << QByteArray("9000");
			QTest::newRow("12") << QByteArray("002a00be0001487f4e8201005f29010042104445445674494447564b3330303031327f494f060a04007f00070202020203864104536d386fc2ce87d5d182a5d4b8074d667e970d64070d1a13cea3ecc00ac07295a5c3ca8af3157a23188484b1fdc1ffecdbc9e40a212738518cb4aa93ffebce1b5f200e444544454d4f50414130303436367f4c12060904007f0007030102025305000513ff075f25060201000402025f2406020100050202655e732d060904007f00070301030180204ebb52e497c3549ca1102ecf55b6626c1afb00d2cdfcad369d37083ece26139e732d060904007f00070301030280206ccf8efd02e71b274c8c4f29122310ef2d7ffdfb4c611fe267f8576da42e7ba25f3740280ddba9541f9e101c9aa31e9de947cd454b7278a1ccf127be8a05ba64c2d9a76c3670fb777c94d722608fb8ab55fdbad4097769057afa9126292c84ca44a687") << QByteArray("9000");
			QTest::newRow("13") << QByteArray("002281a457800a04007f00070202020203830e444544454d4f504141303034363667177315060904007f00070301040253083230323130353036912031a6c7f90fb60be6cf0d6a43ee1ec1771f248a8d4ede513064df37c66dcd84dc") << QByteArray("9000");
			QTest::newRow("14") << QByteArray("008200004024919794477ac11dfdce699665915871ce224d1e988e8722e55ed1d4f1ba02338ed84369f4169eb6b3708ae5da7777d854ef92ecfb940f56c45b6d360d6dd20b") << QByteArray("9000");
			QTest::newRow("15") << QByteArray("00a4020c02011d") << QByteArray("9000");
			QTest::newRow("16") << QByteArray("00b00000ff") << QByteArray("3082076806092a864886f70d010702a082075930820755020103310f300d0609608648016503040202050030820148060804007f0007030201a082013a0482013631820132300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d3017060a04007f0007020205020330090201010201430101ff3017060a04007f0007020205020330090201010201440101003019060904007f000702020502300c060704007f0007010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f6273699000");
			QTest::newRow("17") << QByteArray("00b000ffff") << QByteArray("2e62756e642e64652f6369662f6e70612e786d6c3062060904007f0007020201023052300c060704007f0007010202010d0342000451e404574bd3a639bfeb12bd9b0e4076526ab73dd0998c3b9f5866db4aae96199a436b3f4a6947a64ecc4b75daf289c359ea8e2f4d069957a16ef386dd3f1f3d020141a08204b5308204b130820416a003020102020204d1300a06082a8648ce3d0403043046310b3009060355040613024445310d300b060355040a0c0462756e64310c300a060355040b0c03627369311a301806035504030c115445535420637363612d6765726d616e79301e170d3139313231373039313730335a170d33303037313732333539359000");
			QTest::newRow("18") << QByteArray("00b001feff") << QByteArray("395a306d310b3009060355040613024445311d301b060355040a0c1442756e646573647275636b6572656920476d6248310d300b06035504051304303039363130302e06035504030c275445535420446f63756d656e74205369676e6572204964656e7469747920446f63756d656e7473308201b53082014d06072a8648ce3d020130820140020101303c06072a8648ce3d01010231008cb91e82a3386d280f5d6f7e50e641df152f7109ed5456b412b1da197fb71123acd3a729901d1a71874700133107ec53306404307bc382c63d8c150c3c72080ace05afa0c2bea28e4fb22787139165efba91f90f8aa5814a503ad4eb04a8c7dd22ce2826043004a89000");
			QTest::newRow("19") << QByteArray("00b002fdff") << QByteArray("c7dd22ce28268b39b55416f0447c2fb77de107dcd2a62e880ea53eeb62d57cb4390295dbc9943ab78696fa504c110461041d1c64f068cf45ffa2a63a81b7c13f6b8847a3e77ef14fe3db7fcafe0cbd10e8e826e03436d646aaef87b2e247d4af1e8abe1d7520f9c2a45cb1eb8e95cfd55262b70b29feec5864e19c054ff99129280e4646217791811142820341263c53150231008cb91e82a3386d280f5d6f7e50e641df152f7109ed5456b31f166e6cac0425a7cf3ab6af6b7fc3103b883202e9046565020101036200047059bf6998fc82f8c2cdb104ec0086a034d2793f6b361c8b97b937a45d572ac1f9f8edda31567c6566e9ea00d640b20c3aa621a89000");
			QTest::newRow("20") << QByteArray("00b003fcff") << QByteArray("ddbf9302dfde77bffc8a819d6624882f1fd1dd37e92ef3bb3df969320a0820ba551aa69f265f7ca0399898b0a382016d30820169301f0603551d23041830168014539db1872aac9193d76392ee80d9e5996cf99b3b301d0603551d0e04160414393413665ab3a2223a7d5df3c881adf0d989211d300e0603551d0f0101ff040403020780302b0603551d1004243022800f32303139313231373039313730335a810f32303230303731373233353935395a30160603551d20040f300d300b060904007f000703010101302d0603551d1104263024821262756e646573647275636b657265692e6465a40e300c310a300806035504070c014430510603551d129000");
			QTest::newRow("21") << QByteArray("00b004fbff") << QByteArray("044a30488118637363612d6765726d616e79406273692e62756e642e6465861c68747470733a2f2f7777772e6273692e62756e642e64652f63736361a40e300c310a300806035504070c01443019060767810801010602040e300c02010031071301411302494430350603551d1f042e302c302aa028a0268624687474703a2f2f7777772e6273692e62756e642e64652f746573745f637363615f63726c300a06082a8648ce3d0403040381880030818402401c6add5bacb553dcd7c342e830883311c9356d70f7d2377328297447b524b6544733a6812460f595b34e471fa28991eb4f362b3d448bc424393c218fb126685c0240201831ce3fcdb726ec769000");
			QTest::newRow("22") << QByteArray("00b005faff") << QByteArray("a7dd4df23d6af890baca98bca701fe1910bc4ad4d7222828b2abbed36d9836cb23ae3cb18ab25527e0c0d217a44042c1d56ce2e6e3223182013830820134020101304c3046310b3009060355040613024445310d300b060355040a0c0462756e64310c300a060355040b0c03627369311a301806035504030c115445535420637363612d6765726d616e79020204d1300d06096086480165030402020500a05a301706092a864886f70d010903310a060804007f0007030201303f06092a864886f70d01090431320430e2442240adca4763689f013cf9e96929f65f060f2cd1bb5eb2b98a9046036f03b8b68c009ffec3290bb9541150891f82300c06082a9000");
			QTest::newRow("23") << QByteArray("00b006f9ff") << QByteArray("8648ce3d04030305000466306402302f2e5c071d02843cff3b4e0162f22bb80bb48cb378483652f89df85b565313c4622616c58cadf9c693e9d4b6191c51ac02301f89891bc6a6b496100010c6b71ec61e3088b64f52df774e601edd33b489e1383dca4ffd5f47218fa948784ef2688734a1006282");
			QTest::newRow("24") << QByteArray("002241a40f800a04007f00070202030202840141") << QByteArray("9000");
			QTest::newRow("25") << QByteArray("00860000457c4380410431a6c7f90fb60be6cf0d6a43ee1ec1771f248a8d4ede513064df37c66dcd84dc9e5f3a6792836d609e48e005e019233364e0d8afb9c64404e03c1a0d9f2ddf4300") << QByteArray("7c148108ce16f3e5f595bafe82088e852af31ba64f9e9000");
			QTest::newRow("26") << QByteArray("0ca4040c1d87110124c4aeff9a73a11e2dc460fd11f061788e088fe486d17c44280600") << QByteArray("990290008e083e2e2460ca83cc009000");
			QTest::newRow("27") << QByteArray("8c2080001d871101c593d6d50d7324773cc1a639ef0ce6678e08fc12d9d912de70bb00") << QByteArray("990290008e08a8c37018d3757d1b9000");
			QTest::newRow("28") << QByteArray("0c2241a41d8711012e6098f476be74dce3421f01760ea8c68e083159c57d9625bb5a00") << QByteArray("990290008e08cb24093d591f719d9000");
			QTest::newRow("29") << QByteArray("0c86000000014287820131011c77d95d0138e30e17ef86e8deae1606fe9b69a30d01ee6389a91c2bcba3beb6f94ffda6e078166f0f1b0c0db679921dcfe0157de266c197d1bf020b4213a66589ce0e11be420e817051e0cabd04e163274e83ac5ba2b0009d6ade20c984f2dcc599510b8a922d3696ae7c1c2ee0fa9b5c1152880b687f19957b75b67571e9dcf95a6d7ffd1a90d2154a8e357a86af045dee923f3d83297513c06a489a6e00fa8b68269b150f699b0b706023cbe54a754207bb4045a235051a33e6f10bc0da9c18b618cddb3aa2031adfab9b16178c05d8263895354212f60fe35a6e34ab43bc3d129a7f20f0b7a0bd57d69880de8aca339b85fd9fe82816e8f09b2dd08353e311f7ed2b4ac40f2b460ec4bf87f1313b76fee24c309df9081806df304a915f5f7aa73029e1a8593cac687e3b032dd9c49701248e083416581f561486820000") << QByteArray("873101fd75cf852856b64fc97f7e7f0e4f459e93291166c6ffa382f2bb903262915e6b3104483d3eb921f5ec0a2a32c3a1de3c990290008e0891d14e843acc5b479000");
			QTest::newRow("30") << QByteArray("0cb0810000000e970200008e086a86f65100b5b9200000") << QByteArray("8711017624ee2c2133bf7f338e883e2423d096990290008e08046fa5e87199559a9000");
			QTest::newRow("31") << QByteArray("0cb0820000000e970200008e082241594fff26a9ae0000") << QByteArray("87110198dc5c89bbe28dfaae5a756f726ded2d990290008e0823c5c083b5ce896f9000");
			QTest::newRow("32") << QByteArray("0cb0830000000e970200008e082557c04799a5dea70000") << QByteArray("871101ce871d4f4166832363fc49815679a8d0990290008e085e907d69bccfc4559000");
			QTest::newRow("33") << QByteArray("0cb0840000000e970200008e0854f0e9c5f8d671840000") << QByteArray("8711011c5c5d8d23ca0405d30477e3d4a2d0d5990290008e0846a6a4a480827fc89000");
			QTest::newRow("34") << QByteArray("0cb0850000000e970200008e08c6775c7b84699a810000") << QByteArray("871101d4ee858b3e2d4775c45810e2439dcdd8990290008e08791edb04da98fe049000");
			QTest::newRow("35") << QByteArray("0cb0860000000e970200008e0834e1d7b1d368d0770000") << QByteArray("871101ba5e89277339bb3b4540df9121c60596990290008e08a3f08d10e5d4b47d9000");
			QTest::newRow("36") << QByteArray("0cb0870000000e970200008e08484b788967dfc8130000") << QByteArray("871101ee9645a732e53abfa195173aa494ac82990290008e08ae7653c115bd0e2a9000");
			QTest::newRow("37") << QByteArray("0cb0880000000e970200008e08fa0a60aa82dff6c10000") << QByteArray("871101113000f3f5aa2e24388bd80c6ac0f41c990290008e087381fb9fbd64876e9000");
			QTest::newRow("38") << QByteArray("0cb0890000000e970200008e082162d8400be20a220000") << QByteArray("8711018039e5cbbbde525fece96f60106bd6bc990290008e08462867fe73cd59979000");
			QTest::newRow("39") << QByteArray("0cb08a0000000e970200008e089d294e56dacb07070000") << QByteArray("99026a828e084b1b3ed3e46349ef6a82");
			QTest::newRow("40") << QByteArray("0cb08d0000000e970200008e081813f271d84a41fd0000") << QByteArray("871101b1360c1e45870a8a8f9f5b27048afc28990290008e0860090cf0b9925e519000");
			QTest::newRow("41") << QByteArray("0cb0910000000e970200008e08f0cbfd9b0ef1a5bc0000") << QByteArray("8782019101d6a95285135417291caaaf1eadcf104cf39b99f06da15983a33331e31f69fa15714b6ee0fee51af85bf983ad9424f036bd62c5199335be74af254f70c6bf80c4901a0cb909102fca3b0d20c5fe293f295d650f7f61b9e1ad9c97faf2e5eff00e7700b311ea423a17eca410e383dcdfbd956a68acca3fe61f5081153e337836450b5787108fe41765832f6c2c5d88753c2816c97af5c5c8dd5a54c2e00cc89eac754ccb163f043993efbc1e1ff3f3eb95698a7cca39d6ca94f053a9a95d878cfb3a6dfe5048169bb023461bd0d0ceb0ba0d3b3069548399724dbce76d23b1a24e6c2608b68c4187cd1399b76d9edfc37b5fb42d19928178a0b86d91b4ddf03777e4c7548756bb8ad7cdf5a118c1feddcf6840dd3e6718913ce093817c79e795689acce0806208b5cd78d751b6d3b3201784f23ed1b25a2dd62638e20626691b671bdc0e9e5eee4ed0cd28a00d61ed18170475ebe7424b414f5c3ea6c94235474d3e65458941ba72129d9982ab013d1d059bb73d27af6412f3f10e9b3b4920d1d59942e76bfc88c614aee974086dfe3803990290008e08cb0e6a7f5b1dcc139000");
			QTest::newRow("42") << QByteArray("0cb0930000000e970200008e0885286c526b035db20000") << QByteArray("99026a828e08aa1e688ca4ac4c066a82");
			QTest::newRow("43") << QByteArray("0022c1a412800a04007f0007020204020283010384010d") << QByteArray("9000");
		}


		void fullAuthentication()
		{
			QFETCH(QByteArray, command);
			QFETCH(QByteArray, response);

			const auto cmd = CommandApdu(QByteArray::fromHex(command));
			QCOMPARE(mSecureMessagingCard->decrypt(mSecureMessagingTerminal->encrypt(cmd)), cmd);

			const auto rsp = ResponseApdu(QByteArray::fromHex(response));
			QCOMPARE(mSecureMessagingTerminal->decrypt(mSecureMessagingCard->encrypt(rsp)), rsp);
		}


};

QTEST_GUILESS_MAIN(test_SecureMessaging)
#include "test_SecureMessaging.moc"
