/*!
 * \copyright Copyright (c) 2019-2020 Governikus GmbH & Co. KG, Germany
 */


#include "Random.h"

#include "Randomizer.h"
#include "SingletonHelper.h"

#include <random>

using namespace governikus;

defineSingleton(Random)

Random & Random::getInstance()
{
	return *Instance;
}


int Random::randomInt(int pLowerBound, int pUpperBound) const
{
	std::uniform_int_distribution<int> distribution(pLowerBound, pUpperBound);
	return distribution(Randomizer::getInstance().getGenerator());
}
