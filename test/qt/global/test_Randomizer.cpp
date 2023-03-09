/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
		void defaultSeed()
		{
			QVERIFY(Randomizer::getInstance().getGenerator().default_seed == 5489u);
		}


		void secureSeeded()
		{
			Randomizer& randomizer = Randomizer::getInstance();
			QVERIFY(randomizer.isSecureRandom());
		}


		void defaultSeedNotUsed()
		{
			std::mt19937 unseeded;
			std::mt19937_64 unseeded64;

			auto& randomizer = Randomizer::getInstance();

			for (int i = 0; i < 10; ++i)
			{
				const auto fromRandomizer = randomizer.getGenerator()();
				QVERIFY(fromRandomizer != unseeded());
				QVERIFY(fromRandomizer != unseeded64());
			}
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


		void universalBuffer()
		{
			Randomizer::UniversalBuffer buffer;
			QCOMPARE(buffer.get(), 0);

			buffer.set(666);
			QCOMPARE(buffer.get(), 666);

			memcpy(buffer.data, "1234567", sizeof(buffer.data));
			QCOMPARE(buffer.get(), 15540725856023089);

			buffer.set(13847469359445559);
			QCOMPARE(QLatin1String(reinterpret_cast<char*>(buffer.data)), QLatin1String("7654321"));
		}


};

QTEST_GUILESS_MAIN(test_Randomizer)
#include "test_Randomizer.moc"
