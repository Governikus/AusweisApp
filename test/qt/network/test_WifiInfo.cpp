/*!
 * \brief Unit tests for \ref WifiInfo
 *
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WifiInfo.h"

#include <QtTest>

Q_DECLARE_METATYPE(QHostAddress)

using namespace governikus;

class test_WifiInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void checkIpAddresses_data()
		{
			QTest::addColumn<QHostAddress>("ip");
			QTest::addColumn<bool>("isPrivate");

			QTest::newRow("empty") << QHostAddress() << false;
			QTest::newRow("localhost") << QHostAddress("127.0.0.1") << false;
			QTest::newRow("linkLocal") << QHostAddress("169.254.0.1") << false;

			QTest::newRow("A lower") << QHostAddress("10.0.0.0") << true;
			QTest::newRow("A upper") << QHostAddress("10.255.255.255") << true;

			QTest::newRow("B lower over") << QHostAddress("172.15.255.255") << false;
			QTest::newRow("B lower") << QHostAddress("172.16.0.0") << true;
			QTest::newRow("B upper") << QHostAddress("172.31.255.255") << true;
			QTest::newRow("B upper over") << QHostAddress("172.32.0.0") << false;

			QTest::newRow("C lower") << QHostAddress("192.168.0.0") << true;
			QTest::newRow("C upper") << QHostAddress("192.168.255.255") << true;

			QTest::newRow("fritzbox") << QHostAddress("192.168.178.0") << true;
			QTest::newRow("fritzbox range") << QHostAddress("192.168.178.255") << true;

			QTest::newRow("google dns") << QHostAddress("8.8.8.8") << false;
			QTest::newRow("governikus") << QHostAddress("194.31.70.66") << false;
			QTest::newRow("telekom") << QHostAddress("91.39.48.2") << false;
			QTest::newRow("vodafone mobile") << QHostAddress("100.90.174.1") << false;
			QTest::newRow("provider") << QHostAddress("100.64.0.0") << false;
		}


		void checkIpAddresses()
		{
			QFETCH(QHostAddress, ip);
			QFETCH(bool, isPrivate);

			QCOMPARE(WifiInfo::isPrivateIp(ip), isPrivate);
		}


};

QTEST_GUILESS_MAIN(test_WifiInfo)
#include "test_WifiInfo.moc"
