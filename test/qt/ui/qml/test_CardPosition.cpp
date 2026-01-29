/**
 * Copyright (c) 2025-2026 Governikus GmbH & Co. KG, Germany
 */

#include "CardPosition.h"

#include <QtTest>


using namespace governikus;


class test_CardPosition
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void defaultValues()
		{
			CardPosition cardPosition;
			QCOMPARE(cardPosition.getXPosition(), 0.0);
			QCOMPARE(cardPosition.getYPosition(), 0.0);
			QCOMPARE(cardPosition.getRotation(), 0.0);
			QCOMPARE(cardPosition.getZPosition(), -1);
		}


		void values()
		{
			CardPosition cardPosition({0.1, 0.2, 0.3, 1});
			QCOMPARE(cardPosition.getXPosition(), 0.1);
			QCOMPARE(cardPosition.getYPosition(), 0.2);
			QCOMPARE(cardPosition.getRotation(), 0.3);
			QCOMPARE(cardPosition.getZPosition(), 1);
		}


		void comparator()
		{
			QVERIFY(CardPosition({0.1, 0.2, 0.3, 1}) == CardPosition({0.1, 0.2, 0.3, 1}));
			QVERIFY(CardPosition({0.0, 0.2, 0.3, 1}) != CardPosition({0.1, 0.2, 0.3, 1}));
			QVERIFY(CardPosition({0.1, 0.0, 0.3, 1}) != CardPosition({0.1, 0.2, 0.3, 1}));
			QVERIFY(CardPosition({0.1, 0.2, 0.0, 1}) != CardPosition({0.1, 0.2, 0.3, 1}));
			QVERIFY(CardPosition({0.1, 0.2, 0.3, -1}) != CardPosition({0.1, 0.2, 0.3, 1}));
		}


};

QTEST_MAIN(test_CardPosition)
#include "test_CardPosition.moc"
