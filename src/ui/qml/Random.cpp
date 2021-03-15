/*!
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */


#include "Random.h"

#include "Randomizer.h"

#include <random>

using namespace governikus;

int Random::randomInt(int pLowerBound, int pUpperBound) const
{
	std::uniform_int_distribution<int> distribution(pLowerBound, pUpperBound);
	return distribution(Randomizer::getInstance().getGenerator());
}
