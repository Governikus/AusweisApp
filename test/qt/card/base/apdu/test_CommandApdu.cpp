/*!
 * \brief Tests for the class CommandApdu.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "apdu/CommandApdu.h"

#include "LogHandler.h"

#include <QtCore>
#include <QtTest>

using namespace governikus;

class test_CommandApdu
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void initTestCase()
		{
			Env::getSingleton<LogHandler>()->init();
		}


		void cleanup()
		{
			Env::getSingleton<LogHandler>()->resetBacklog();
		}


		void test_getLe()
		{
			QByteArray array(7, 0);
			CommandApdu apdu = CommandApdu(array);
			QCOMPARE(apdu.getLe(), 0x010000);
		}


		void test_getP2_data()
		{
			QTest::addColumn<char>("p2");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("empty") << '\0' << QByteArray();
			QTest::newRow("abc") << '\0' << QByteArray("abc");
			QTest::newRow("input") << 'u' << QByteArray("input");
		}


		void test_getP2()
		{
			QFETCH(QByteArray, data);
			QFETCH(char, p2);

			CommandApdu apdu = CommandApdu(data);
			QCOMPARE(apdu.getP2(), p2);
		}


		void test_getP1_data()
		{
			QTest::addColumn<char>("p1");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("empty") << '\0' << QByteArray();
			QTest::newRow("abc") << 'c' << QByteArray("abc");
			QTest::newRow("input") << 'p' << QByteArray("input");
		}


		void test_getP1()
		{
			QFETCH(QByteArray, data);
			QFETCH(char, p1);

			CommandApdu apdu = CommandApdu(data);
			QCOMPARE(apdu.getP1(), p1);
		}


		void test_Ins_data()
		{
			QTest::addColumn<Ins>("ins");

			const auto& instructions = Enum<Ins>::getList();
			for (const auto& ins : instructions)
			{
				QTest::newRow(Enum<Ins>::getName(ins).data()) << ins;
			}
		}


		void test_Ins()
		{
			QFETCH(Ins, ins);

			CommandApdu apdu(ins, 0, 0);
			QCOMPARE(apdu.getINS(), ins);
			QCOMPARE(CommandApdu(QByteArray(apdu)).getINS(), ins);
		}


		void test_Ins_unknown_data()
		{
			QTest::addColumn<QByteArray>("header");
			QTest::addColumn<QString>("logMsg");

			QTest::newRow("00") << QByteArray("00000000") << QString("Unknown INS value, returning UNKNOWN, value: 0x0");
			QTest::newRow("42") << QByteArray("00420000") << QString("Unknown INS value, returning UNKNOWN, value: 0x42");
			QTest::newRow("AA") << QByteArray("00AA0000") << QString("Unknown INS value, returning UNKNOWN, value: 0xaa");
			QTest::newRow("FF") << QByteArray("00FF0000") << QString("Unknown INS value, returning UNKNOWN, value: 0xff");
		}


		void test_Ins_unknown()
		{
			QFETCH(QByteArray, header);
			QFETCH(QString, logMsg);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			CommandApdu apdu(QByteArray::fromHex(header));
			QCOMPARE(apdu.getINS(), Ins::UNKNOWN);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains(logMsg));
		}


		void test_CommandChaining_data()
		{
			QTest::addColumn<char>("cla");
			QTest::addColumn<bool>("enabled");

			for (int i = -128; i < 128; ++i)
			{
				QTest::newRow(QByteArray(1, static_cast<char>(i)).toHex().data()) << static_cast<char>(i) << ((i & 0x10) == 0x10);
			}
		}


		void test_CommandChaining()
		{
			QFETCH(char, cla);
			QFETCH(bool, enabled);

			CommandApdu apdu(QByteArray(4, cla));
			QCOMPARE(apdu.isCommandChaining(), enabled);

			apdu.enableCommandChaining();
			QVERIFY(apdu.isCommandChaining());
			QCOMPARE(cla & 0xEF, QByteArray(apdu).at(0) & 0xEF);
		}


		void test_SecureMessaging_data()
		{
			QTest::addColumn<char>("cla");
			QTest::addColumn<bool>("enabled");

			for (int i = -128; i < 128; ++i)
			{
				QTest::newRow(QByteArray(1, static_cast<char>(i)).toHex().data()) << static_cast<char>(i) << ((i & 0x0C) == 0x0C);
			}
		}


		void test_SecureMessaging()
		{
			QFETCH(char, cla);
			QFETCH(bool, enabled);

			CommandApdu apdu(QByteArray(4, cla));
			QCOMPARE(apdu.isSecureMessaging(), enabled);

			apdu.setSecureMessaging(!enabled);
			QCOMPARE(apdu.isSecureMessaging(), !enabled);
			QCOMPARE(cla & 0xF3, QByteArray(apdu).at(0) & 0xF3);

			apdu.setSecureMessaging(enabled);
			QCOMPARE(apdu.isSecureMessaging(), enabled);
			QCOMPARE(cla & 0xF3, QByteArray(apdu).at(0) & 0xF3);
		}


		void test_Oversized()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			CommandApdu apdu1(QByteArray("aicd"), QByteArray(65536, 'w'), 1);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Command data exceeds maximum of 0xffff"));

			CommandApdu apdu2(QByteArray("aicd"), QByteArray("abc"), 65537);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Expected length exceeds maximum value of 0x10000"));
		}


		void test_Invalid_data()
		{
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<QString>("logMessage");

			QTest::newRow("incomplete header 1") << QByteArray::fromHex("00") << QString("Wrong command header size!");
			QTest::newRow("incomplete header 2") << QByteArray::fromHex("0000") << QString("Wrong command header size!");
			QTest::newRow("incomplete header 3") << QByteArray::fromHex("000000") << QString("Wrong command header size!");

			QTest::newRow("unexpected end - short lc - incomplete data") << QByteArray::fromHex("00000000 02 00") << QString("Unexpected end of data");
			QTest::newRow("unexpected end - extended le - incomplete le") << QByteArray::fromHex("00000000 0000") << QString("Unexpected end of data");
			QTest::newRow("unexpected end - extended lc - incomplete data") << QByteArray::fromHex("00000000 000002 00") << QString("Unexpected end of data");

			QTest::newRow("mixed extended / short length") << QByteArray::fromHex("00000000 00000100 00") << QString("Extended length expected");

			QTest::newRow("mixed short / extended length") << QByteArray::fromHex("00000000 0100 0000") << QString("Unexpected additional data: \"00\"");
			QTest::newRow("unexpected additional data - short") << QByteArray::fromHex("00000000 0100 00 00") << QString("Unexpected additional data: \"00\"");
			QTest::newRow("unexpected additional data - extended") << QByteArray::fromHex("00000000 00000100 0000 00") << QString("Unexpected additional data: \"00\"");
		}


		void test_Invalid()
		{
			QFETCH(QByteArray, data);
			QFETCH(QString, logMessage);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			CommandApdu apdu(data);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains(logMessage));
		}


		void test_MessageStructure_data()
		{
			QTest::addColumn<int>("lc");
			QTest::addColumn<int>("le");
			QTest::addColumn<bool>("extendedLength");
			QTest::addColumn<QByteArray>("buffer");

			// Cases according to ISO-7816-4 (First edition, 1995-09), 5.3.2 Decoding conventions for command bodies

			QTest::newRow("1") << 0 << 0 << false << QByteArray::fromHex("01020304");

			QTest::newRow("2S 0   1") << 0 << 1 << false << QByteArray::fromHex("0102030401");
			QTest::newRow("2S 0 255") << 0 << 255 << false << QByteArray::fromHex("01020304FF");
			QTest::newRow("2S 0 256") << 0 << 256 << false << QByteArray::fromHex("0102030400");

			QTest::newRow("3S   1 0") << 1 << 0 << false << QByteArray::fromHex("010203040142");
			QTest::newRow("3S 255 0") << 255 << 0 << false << QByteArray::fromHex("01020304FF") + QByteArray(255, 0x42);

			QTest::newRow("4S   1   1 ") << 1 << 1 << false << QByteArray::fromHex("01020304014201");
			QTest::newRow("4S   1 255") << 1 << 255 << false << QByteArray::fromHex("010203040142FF");
			QTest::newRow("4S   1 256") << 1 << 256 << false << QByteArray::fromHex("01020304014200");
			QTest::newRow("4S 255   1") << 255 << 1 << false << QByteArray::fromHex("01020304FF") + QByteArray(255, 0x42) + QByteArray::fromHex("01");
			QTest::newRow("4S 255 255") << 255 << 255 << false << QByteArray::fromHex("01020304FF") + QByteArray(255, 0x42) + QByteArray::fromHex("FF");
			QTest::newRow("4S 255 256") << 255 << 256 << false << QByteArray::fromHex("01020304FF") + QByteArray(255, 0x42) + QByteArray::fromHex("00");

			QTest::newRow("2E 0   257") << 0 << 257 << true << QByteArray::fromHex("01020304000101");
			QTest::newRow("2E 0 65535") << 0 << 65535 << true << QByteArray::fromHex("0102030400FFFF");
			QTest::newRow("2E 0 65536") << 0 << 65536 << true << QByteArray::fromHex("01020304000000");

			QTest::newRow("3E   256 0") << 256 << 0 << true << QByteArray::fromHex("01020304000100") + QByteArray(256, 0x42);
			QTest::newRow("3E 65535 0") << 65535 << 0 << true << QByteArray::fromHex("0102030400FFFF") + QByteArray(65535, 0x42);

			QTest::newRow("4E   256   257") << 256 << 257 << true << QByteArray::fromHex("01020304000100") + QByteArray(256, 0x42) + QByteArray::fromHex("0101");
			QTest::newRow("4E   256 65535") << 256 << 65535 << true << QByteArray::fromHex("01020304000100") + QByteArray(256, 0x42) + QByteArray::fromHex("FFFF");
			QTest::newRow("4E   256 65536") << 256 << 65536 << true << QByteArray::fromHex("01020304000100") + QByteArray(256, 0x42) + QByteArray::fromHex("0000");
			QTest::newRow("4E 65535   257") << 65535 << 257 << true << QByteArray::fromHex("0102030400FFFF") + QByteArray(65535, 0x42) + QByteArray::fromHex("0101");
			QTest::newRow("4E 65535 65535") << 65535 << 65535 << true << QByteArray::fromHex("0102030400FFFF") + QByteArray(65535, 0x42) + QByteArray::fromHex("FFFF");
			QTest::newRow("4E 65535 65536") << 65535 << 65536 << true << QByteArray::fromHex("0102030400FFFF") + QByteArray(65535, 0x42) + QByteArray::fromHex("0000");
		}


		void test_MessageStructure()
		{
			QFETCH(int, lc);
			QFETCH(int, le);
			QFETCH(bool, extendedLength);
			QFETCH(QByteArray, buffer);
			const QByteArray data(lc, 0x42);

			QSignalSpy logSpy(Env::getSingleton<LogHandler>()->getEventHandler(), &LogEventHandler::fireLog);

			CommandApdu apdu(QByteArray::fromHex("01020304"), data, le);
			QCOMPARE(apdu.getLe(), le);
			QCOMPARE(apdu.getData(), data);
			QCOMPARE(apdu.isExtendedLength(), extendedLength);
			QCOMPARE(apdu, buffer);
			QCOMPARE(logSpy.count(), 0);

			CommandApdu apduParse(buffer);
			QCOMPARE(apduParse.getLe(), le);
			QCOMPARE(apduParse.getData(), data);
			QCOMPARE(apduParse.isExtendedLength(), extendedLength);
			QCOMPARE(apduParse, buffer);
			QCOMPARE(logSpy.count(), 0);
		}


};

QTEST_GUILESS_MAIN(test_CommandApdu)
#include "test_CommandApdu.moc"
