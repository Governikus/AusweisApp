/*!
 * \brief Unit tests for \ref Randomizer
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
			auto& randomizer = Randomizer::getInstance();
			const QSet<unsigned long> defaultSeedValues = {
				3499211612, 581869302, 3890346734, 3586334585, 545404204
			};

			QSet<unsigned long> randomValues;
			for (int i = 0; i < defaultSeedValues.size(); ++i)
			{
				randomValues += randomizer.getGenerator()();
			}

			QVERIFY(randomValues != defaultSeedValues);
		}


};

QTEST_GUILESS_MAIN(test_Randomizer)
#include "test_Randomizer.moc"
