/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore>
#include <QtTest>

#include "asn1/KnownOIDs.h"
#include "pace/SecureMessaging.h"

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
		QScopedPointer<SecureMessaging> mSecureMessaging;
		QScopedPointer<CipherMac> mCipherMac;
		QScopedPointer<SymmetricCipher> mCipher;


		[[nodiscard]] QByteArray concat(std::initializer_list<QByteArray> pList) const
		{
			QByteArray result;
			for (const auto& elem : pList)
			{
				result += elem;
			}
			return result;
		}


		QVector<QByteArray> encryptResponse(const QByteArray& pBuffer, quint32& pSsc) const
		{
			QVector<QByteArray> result(4);

			++pSsc;
			QByteArray ssc = QByteArray(mCipher->getBlockSize() - QByteArray::number(pSsc, 16).size(), 0x00).append(ByteArrayUtil::fromValue(pSsc));

			if (pBuffer.length() > 2)
			{
				QByteArray pData = pBuffer.left(pBuffer.length() - 2);
				mCipher->setIv(QByteArray().fill(0x00, mCipher->getBlockSize()));
				auto iv = mCipher->encrypt(ssc);
				mCipher->setIv(iv);

				const auto padLen = mCipher->getBlockSize() - (pData.size() % mCipher->getBlockSize());
				pData = concat({pData, QByteArray::fromHex("80"), QByteArray(padLen - 1, 0x00)});
				QByteArray encryptedData = mCipher->encrypt(pData).prepend(0x01);
				result[0] = QByteArray().append(char(0x87)).append(ByteArrayUtil::fromValue(encryptedData.size())).append(encryptedData);
			}
			QByteArray pSw = pBuffer.right(2);
			result[1] = QByteArray().append(char(0x99)).append(ByteArrayUtil::fromValue(pSw.size())).append(pSw);
			{
				QByteArray dataToMac = concat({ssc, result[0], result[1]});
				const auto padLen = mCipher->getBlockSize() - (dataToMac.size() % mCipher->getBlockSize());
				dataToMac.append(char(0x80)).append(QByteArray(padLen - 1, 0x00));
				QByteArray mac = mCipherMac->generate(dataToMac);
				result[2] = QByteArray().append(char(0x8E)).append(ByteArrayUtil::fromValue(mac.size())).append(mac);
			}
			result[3] = pSw;

			return result;
		}

	private Q_SLOTS:
		void init()
		{
			auto paceAlgo = toByteArray(KnownOIDs::id_PACE::ECDH::GM_AES_CBC_CMAC_128);
			QByteArray encKey("F1234567890ABCDE");
			QByteArray macKey("1234567890ABCDEF");
			mSecureMessaging.reset(new SecureMessaging(paceAlgo, encKey, macKey));
			mCipherMac.reset(new CipherMac(paceAlgo, macKey));
			mCipher.reset(new SymmetricCipher(paceAlgo, encKey));
		}


		void testNotInitialized()
		{
			const QByteArray empty;
			QByteArray data;
			const CommandApdu command(static_cast<char>(0x10), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::NO_LE);
			SecureMessaging n(empty, empty, empty);
			CommandApdu commandApdu = n.encrypt(command);
			QCOMPARE(commandApdu.getBuffer(), QByteArray());
		}


		void testCommandChainingCLA()
		{
			QByteArray data;
			CommandApdu command(static_cast<char>(0x10), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::NO_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("10102030"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(), QByteArray::fromHex("1c1020300a8e0852d8f5136eddaab800"));
		}


		void testNoDataNoLe()
		{
			QByteArray data;
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::NO_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("00102030"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(), QByteArray::fromHex("0c1020300a8e08fd824409ae18c2f500"));
		}


		void testNoDataShortLe()
		{
			QByteArray data;
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, 8);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("0010203008"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(), QByteArray::fromHex("0c1020300d9701088e080639e0cc69d18d3900"));
		}


		void testNoDataShortMaxLe()
		{
			QByteArray data;
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::SHORT_MAX_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("0010203000"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(), QByteArray::fromHex("0c1020300d9701008e08a361948e0c90464b00"));
		}


		void testNoDataExtendedMaxLe()
		{
			QByteArray data;
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("00102030000000"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(), QByteArray::fromHex("0c10203000000e970200008e088b5e9fdb6cc8d9c70000"));
		}


		void testDataShortMaxLe()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::SHORT_MAX_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("0010203004d0d1d2d300"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(),
					QByteArray::fromHex("0c10203020871101cc30c03dbe01213486b74ce1f48f5c499701008e085f5f415d7bcdb09200"));
		}


		void testDataExtendedMaxLe()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("00102030000004d0d1d2d30000"));
			CommandApdu securedCommand = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand.getBuffer(),
					QByteArray::fromHex("0c102030000021871101cc30c03dbe01213486b74ce1f48f5c49970200008e08e694d258bc80a23f0000"));
		}


		void testSendSequenceCounter()
		{
			QByteArray data = QByteArray::fromHex("D0D1D2D3");
			CommandApdu command(static_cast<char>(0x00), static_cast<char>(0x10), static_cast<char>(0x20), static_cast<char>(0x30), data, CommandApdu::EXTENDED_MAX_LE);
			QCOMPARE(command.getBuffer(), QByteArray::fromHex("00102030000004d0d1d2d30000"));

			CommandApdu securedCommand1 = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand1.getBuffer(),
					QByteArray::fromHex("0c102030000021871101cc30c03dbe01213486b74ce1f48f5c49970200008e08e694d258bc80a23f0000"));

			CommandApdu securedCommand2 = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand2.getBuffer(),
					QByteArray::fromHex("0c10203000002187110140eade1a656a226767536a7cec2ac6f9970200008e08e3bcfba8b3fde2d40000"));

			CommandApdu securedCommand3 = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand3.getBuffer(),
					QByteArray::fromHex("0c1020300000218711010bdf2b6181a9091e6fe0147d8b11069d970200008e08e794e018bd7414b80000"));

			CommandApdu securedCommand4 = mSecureMessaging->encrypt(command);
			QCOMPARE(securedCommand4.getBuffer(),
					QByteArray::fromHex("0c102030000021871101252538e3ebad320fcd01905459f6b6e0970200008e08e5feec033d432ed00000"));
		}


		void testLastIndexOf()
		{
			// Check if lastIndexOf will return the correct result
			// https://bugreports.qt.io/browse/QTBUG-44223

			const char ISO_LEADING_PAD_BYTE = char(0x80);
			QByteArray data = QByteArray::fromHex("109205a006092b864886faad010702a08255913082058d020103310f300d0609608648016503040201050030820148060804007f0007030201a082013a0482013631820132300d060804007f00070202020201023012060a04007f000702020302020201020201413012060a04007f0007020204020202010202010d3017060a04007f0007020205020330090201010201430101ff3017060a04007f0007020205020330090201010201440101003019060904007f000702020502300c060704007f0007010202010d301c060904007f000702020302300c060704007f0007010202010d020141302a060804007f0007020206161e687474703a2f2f62736980");
			QCOMPARE(data.lastIndexOf(ISO_LEADING_PAD_BYTE), 255);
		}


		void testDecrypt_onlyStatusWord()
		{
			QByteArray plainBuffer = QByteArray::fromHex("9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], result[1], result[2], result[3]}));

			ResponseApdu decryptedResponse = mSecureMessaging->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse.getBuffer(), plainBuffer);
		}


		void testDecrypt()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], result[1], result[2], result[3]}));

			ResponseApdu decryptedResponse = mSecureMessaging->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse.getBuffer(), plainBuffer);
		}


		void testDecrypt_updateSendSequenceCounter()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");
			QByteArray plainBuffer2 = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], result[1], result[2], result[3]}));

			QCOMPARE(ssc, 1u);
			ResponseApdu decryptedResponse = mSecureMessaging->decrypt(encryptedResponse);
			QCOMPARE(decryptedResponse.getBuffer(), plainBuffer);

			auto result2 = encryptResponse(plainBuffer2, ssc);
			ResponseApdu encryptedResponse2(concat({result2[0], result2[1], result2[2], result2[3]}));

			QCOMPARE(ssc, 2u);
			ResponseApdu decryptedResponse2 = mSecureMessaging->decrypt(encryptedResponse2);
			QCOMPARE(decryptedResponse2.getBuffer(), plainBuffer2);
		}


		void testDecrypt_WrongSw()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], result[1], result[2], QByteArray::fromHex("6B00")}));

			QVERIFY(mSecureMessaging->decrypt(encryptedResponse).isEmpty());
		}


		void testDecrypt_WrongSecuredSw()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], QByteArray::fromHex("99026B00"), result[2], result[3]}));

			QVERIFY(mSecureMessaging->decrypt(encryptedResponse).isEmpty());
		}


		void testDecrypt_WrongMac()
		{
			QByteArray plainBuffer = QByteArray::fromHex("0102030405060708090A9000");

			quint32 ssc = 0;
			auto result = encryptResponse(plainBuffer, ssc);
			ResponseApdu encryptedResponse(concat({result[0], result[1], QByteArray::fromHex("8E080102030404030201"), result[3]}));

			QVERIFY(mSecureMessaging->decrypt(encryptedResponse).isEmpty());
		}


};

QTEST_GUILESS_MAIN(test_SecureMessaging)
#include "test_SecureMessaging.moc"
