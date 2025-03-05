/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfoFactory.h"

#include <QtTest>

using namespace governikus;

class test_CardInfoFactory
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_checkEfCardAccess_data()
		{
			QTest::addColumn<QByteArray>("efCardAccessBytes");
			QTest::addColumn<bool>("expectedResult");

			QTest::newRow("No PACEInfo with domain parameters")
				<< QByteArray()
				<< false;
			QTest::newRow("PACEInfo With domain parameters ")
				<< QByteArray("31 14"
							  "        30 12"
							  "            06 0A 04007F00070202040202"
							  "            02 01 02"
							  "            02 01 08")
				<< true;
			QTest::newRow("Smart-eID without Security Mechanism")
				<< QByteArray("31 14"
							  "        30 12"
							  "            06 0A 04007F00070202040202"
							  "            02 01 02"
							  "            02 01 08")
				<< true;
			QTest::newRow("Smart-eID with Security Mechanism")
				<< QByteArray("31 24"
							  "        30 12"
							  "            06 0A 04007F00070202040202"
							  "            02 01 02"
							  "            02 01 08"
							  "        30 0E"
							  "            06 0A 04007F00070302030201"
							  "            05 00")
				<< true;
		}


		void test_checkEfCardAccess()
		{
			QFETCH(QByteArray, efCardAccessBytes);
			QFETCH(bool, expectedResult);

			auto efCardAccess = EFCardAccess::fromHex(efCardAccessBytes);
			QCOMPARE(CardInfoFactory::checkEfCardAccess(efCardAccess), expectedResult);
		}


};

QTEST_GUILESS_MAIN(test_CardInfoFactory)
#include "test_CardInfoFactory.moc"
