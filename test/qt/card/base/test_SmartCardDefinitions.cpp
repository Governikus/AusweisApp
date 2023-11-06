/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

#include "SmartCardDefinitions.h"

#include <QtTest>


using namespace governikus;


class test_SmartCardDefinitions
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void conversion_data()
		{
			QTest::addColumn<AcceptedEidType>("acceptedType");
			QTest::addColumn<MobileEidType>("mobileType");

			QTest::newRow("CARD_CERTIFIED") << AcceptedEidType::CARD_CERTIFIED << MobileEidType::UNKNOWN;
			QTest::newRow("SE_CERTIFIED") << AcceptedEidType::SE_CERTIFIED << MobileEidType::SE_CERTIFIED;
			QTest::newRow("SE_ENDORSED") << AcceptedEidType::SE_ENDORSED << MobileEidType::SE_ENDORSED;
			QTest::newRow("HW_KEYSTORE") << AcceptedEidType::HW_KEYSTORE << MobileEidType::HW_KEYSTORE;
		}


		void conversion()
		{
			QFETCH(AcceptedEidType, acceptedType);
			QFETCH(MobileEidType, mobileType);

			QCOMPARE(Enum<AcceptedEidType>::getValue(acceptedType), Enum<MobileEidType>::getValue(mobileType));
			QCOMPARE(static_cast<MobileEidType>(acceptedType), mobileType);
			QCOMPARE(static_cast<AcceptedEidType>(mobileType), acceptedType);
		}


};

QTEST_GUILESS_MAIN(test_SmartCardDefinitions)
#include "test_SmartCardDefinitions.moc"
