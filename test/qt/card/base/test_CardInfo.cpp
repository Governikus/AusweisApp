/**
 * Copyright (c) 2018-2024 Governikus GmbH & Co. KG, Germany
 */

#include "CardInfo.h"

#include <QtTest>

using namespace Qt::Literals::StringLiterals;
using namespace governikus;

class test_CardInfo
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void test_CardTypeString_data()
		{
			QTest::addColumn<CardType>("type");
			QTest::addColumn<QLatin1String>("output");

			QTest::newRow("none") << CardType::NONE << "not inserted"_L1;
			QTest::newRow("unknown") << CardType::UNKNOWN << "unknown type"_L1;
			QTest::newRow("eid-card") << CardType::EID_CARD << "ID card (PA/eAT/eID)"_L1;
			QTest::newRow("smart-eid") << CardType::SMART_EID << "Smart-eID"_L1;
		}


		void test_CardTypeString()
		{
			QFETCH(CardType, type);
			QFETCH(QLatin1String, output);

			const CardInfo info(type, QSharedPointer<EFCardAccess>(), 3, false, false);

			QCOMPARE(info.getCardTypeString(), output);
		}


		void test_getMobileEidType_data()
		{
			QTest::addColumn<QByteArray>("efCardAccessBytes");
			QTest::addColumn<CardType>("cardType");
			QTest::addColumn<MobileEidType>("expectedResult");

			QTest::newRow("Not set")
				<< QByteArray()
				<< CardType::UNKNOWN
				<< MobileEidType::UNKNOWN;

			QTest::newRow("SE_CERTIFIED")
				<< QByteArray("31 10"
							  "        30 0E"
							  "            06 0A 04007F00070302030201"
							  "            05 00")
				<< CardType::SMART_EID
				<< MobileEidType::SE_CERTIFIED;
			QTest::newRow("SE_ENDORSED")
				<< QByteArray("31 10"
							  "        30 0E"
							  "            06 0A 04007F00070302030202"
							  "            05 00")
				<< CardType::SMART_EID
				<< MobileEidType::SE_ENDORSED;
			QTest::newRow("HW_KEYSTORE")
				<< QByteArray("31 10"
							  "        30 0E"
							  "            06 0A 04007F00070302030203"
							  "            05 00")
				<< CardType::SMART_EID
				<< MobileEidType::HW_KEYSTORE;
			QTest::newRow("Physical eID card")
				<< QByteArray("31 14"
							  "        30 12"
							  "            06 0A 04007F00070202030202"
							  "            02 01 02"
							  "            02 01 08")
				<< CardType::EID_CARD
				<< MobileEidType::UNKNOWN;
		}


		void test_getMobileEidType()
		{
			QFETCH(QByteArray, efCardAccessBytes);
			QFETCH(CardType, cardType);
			QFETCH(MobileEidType, expectedResult);

			const auto efCardAccess = EFCardAccess::fromHex(efCardAccessBytes);
			const CardInfo info = efCardAccessBytes.isEmpty()
					? CardInfo(cardType)
					: CardInfo(cardType, efCardAccess, 3, false, false);
			QCOMPARE(info.getMobileEidType(), expectedResult);
		}


		void test_RetryCounterDeterminated()
		{
			const CardInfo info1(CardType::EID_CARD);
			QVERIFY(!info1.isRetryCounterDetermined());

			const CardInfo info2(CardType::EID_CARD, QSharedPointer<EFCardAccess>(), 3, false, false);
			QVERIFY(info2.isRetryCounterDetermined());
		}


};

QTEST_GUILESS_MAIN(test_CardInfo)
#include "test_CardInfo.moc"
