/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Unit tests for \ref Randomizer
 */

#include "Randomizer.h"

#include <QSet>
#include <QtTest>
#include <random>

using namespace governikus;

class test_Randomizer
	: public QObject
{
	Q_OBJECT

	private Q_SLOTS:
		void secureSeeded()
		{
			Randomizer& randomizer = Randomizer::getInstance();
			QVERIFY(randomizer.isSecureRandom());
		}


		void createUuid()
		{
			auto& randomizer = Randomizer::getInstance();

			const auto& uuid1 = randomizer.createUuid();
			QVERIFY(!uuid1.isNull());

			const auto& uuid2 = randomizer.createUuid();
			QVERIFY(!uuid2.isNull());

			QVERIFY(uuid1 != uuid2);
		}
};

QTEST_GUILESS_MAIN(test_Randomizer)
#include "test_Randomizer.moc"
