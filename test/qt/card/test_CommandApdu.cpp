/*!
 * \brief Tests for the class CommandApdu.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include <QtCore/QtCore>
#include <QtTest/QtTest>

#include "Apdu.h"

#include "TestFileHelper.h"

using namespace governikus;

class test_CommandApdu
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void testGetLc()
		{
			QByteArray data = QByteArray::fromHex(
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
			CommandApdu apdu(CommandApdu::CLA, 0x2a, 0, char(190), data);
			QCOMPARE(apdu.getLc(), 438);
			QCOMPARE(apdu.getLe(), 0);
		}


		void testISecureMessaging()
		{
			CommandApdu secureApdu(QByteArray::fromHex("0CB0840000000E970200008E087ACC9D93FCEAF4660000"));
			CommandApdu insecureApdu(QByteArray::fromHex("0022c1a427800a04007f0007020204020283010384010d7f4c12060904007f0007030102025305000111db05"));

			QVERIFY(CommandApdu::isSecureMessaging(secureApdu.getBuffer()));
			QVERIFY(!CommandApdu::isSecureMessaging(insecureApdu.getBuffer()));
		}


};

QTEST_GUILESS_MAIN(test_CommandApdu)
#include "test_CommandApdu.moc"
