/*!
 * \brief Tests for the class CommandApdu.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "CommandApdu.h"

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


		void testGetLc()
		{
			QByteArray data1 = QByteArray::fromHex(
					"7f4e82016e5f290100420e44454356434165494430303130327f4982011d060a04007f000702020202038120a9fb57d"
					"ba1eea9bc3e660a909d838d726e3bf623d52620282013481d1f6e537782207d5a0975fc2c3057eef67530417affe7fb"
					"8055c126dc5c6ce94a4b44f330b5d9832026dc5c6ce94a4b44f330b5d9bbd77cbf958416295cf7e1ce6bccdc18ff8c0"
					"7b68441048bd2aeb9cb7e57cb2c4b482ffc81b7afb9de27e1e3bd23c23a4453bd9ace3262547ef835c3dac4fd97f846"
					"1a14611dc9c27745132ded8e545c1d54c72f0469978520a9fb57dba1eea9bc3e660a909d838d718c397aa3b561a6f79"
					"01e0e82974856a78641048925419fc7f194922cfc6b8dd25ae6a19c1b59216e6cf06270e5d75cfd64205f55cf867bbf"
					"efeefd6e680e1fd197f18ab684484901362568efc9adb5c6018d728701015f200e44454356434165494430303130337"
					"f4c12060904007f0007030102025305fc0f13ffff5f25060102010200035f24060105010200035f37404d6f08a86a4f"
					"18409f6685387dd3c6a7ff5d68ea4f7714a861bbb3bb721d05d3014adf1763c9292f715d8e94ee9b3e1b73ab1382414"
					"ebf39dfb3b0fb6c09dbeb");

			CommandApdu apdu1(CommandApdu::CLA, 0x2a, 0, char(190), data1);

			QCOMPARE(apdu1.getLc(), 438);
			QCOMPARE(apdu1.getLe(), 0);
		}


		void test_getLc_data()
		{
			QTest::addColumn<QByteArray>("data");
			QTest::addColumn<int>("lc");

			QTest::newRow("abc") << QByteArray("abc") << 0;
			QTest::newRow("abcdef") << QByteArray("abcdef") << 101;
		}


		void test_getLc()
		{
			QFETCH(QByteArray, data);
			QFETCH(int, lc);

			CommandApdu apdu = CommandApdu(data);
			QCOMPARE(apdu.getLc(), lc);
		}


		void test_LcExtendedLength()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			QByteArray array = QByteArray::fromHex("010101010001");
			CommandApdu apdu = CommandApdu(array);
			QCOMPARE(apdu.getLc(), 0);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Cannot determine Lc, returning 0"));
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


		void test_getINS_data()
		{
			QTest::addColumn<char>("ins");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("empty") << '\0' << QByteArray();
			QTest::newRow("abc") << 'b' << QByteArray("abc");
			QTest::newRow("input") << 'n' << QByteArray("input");
		}


		void test_getINS()
		{
			QFETCH(QByteArray, data);
			QFETCH(char, ins);

			CommandApdu apdu = CommandApdu(data);
			QCOMPARE(apdu.getINS(), ins);
		}


		void test_getCLA_data()
		{
			QTest::addColumn<char>("cla");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("empty") << '\0' << QByteArray();
			QTest::newRow("abc") << 'a' << QByteArray("abc");
			QTest::newRow("input") << 'i' << QByteArray("input");
		}


		void test_getCLA()
		{
			QFETCH(QByteArray, data);
			QFETCH(char, cla);

			CommandApdu apdu = CommandApdu(data);
			QCOMPARE(apdu.getCLA(), cla);
		}


		void test_SecureMessaging_data()
		{
			QTest::addColumn<bool>("secure");
			QTest::addColumn<QByteArray>("data");

			QTest::newRow("null") << false << QByteArray();
			QTest::newRow("empty") << false << QByteArray("");
			QTest::newRow("data") << false << QByteArray("data");
			QTest::newRow("0c") << true << QByteArray::fromHex("0c");
		}


		void test_SecureMessaging()
		{
			CommandApdu apdu = CommandApdu(QByteArray());
			QFETCH(bool, secure);
			QFETCH(QByteArray, data);

			QCOMPARE(apdu.isSecureMessaging(data), secure);
		}


		void test_InvalidCommandApdu()
		{
			QSignalSpy logSpy(Env::getSingleton<LogHandler>(), &LogHandler::fireLog);

			CommandApdu apdu1 = CommandApdu('a', 'i', 'c', 'd', QByteArray(65536, 'w'), 1);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Command data exceeds maximum of 0xFFFF bytes"));

			CommandApdu apdu2 = CommandApdu('a', 'i', 'c', 'd', QByteArray("abc"), 65539);
			QCOMPARE(logSpy.count(), 1);
			QVERIFY(logSpy.takeFirst().at(0).toString().contains("Expected length exceeds maximum value of 0x010000"));
		}


};

QTEST_GUILESS_MAIN(test_CommandApdu)
#include "test_CommandApdu.moc"
