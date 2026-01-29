/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#include "Randomizer.h"

#include <QtConcurrent>
#include <QtTest>

using namespace governikus;

class test_Randomizer
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void createBytes()
		{
			auto& randomizer = Randomizer::getInstance();
			for (int i = 0; i <= 16; ++i)
			{
				QCOMPARE(randomizer.createBytes(i).size(), i);
			}
		}


		void createUuid()
		{
			auto& randomizer = Randomizer::getInstance();

			const auto& uuid1 = randomizer.createUuid();
			QVERIFY(!uuid1.isNull());

			const auto& uuid2 = randomizer.createUuid();
			QVERIFY(!uuid2.isNull());

			QCOMPARE_NE(uuid1, uuid2);
		}


};

QTEST_GUILESS_MAIN(test_Randomizer)
#include "test_Randomizer.moc"
