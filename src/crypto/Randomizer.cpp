/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#include "Randomizer.h"

#include "SingletonHelper.h"

#include <QByteArray>
#include <QtEndian>

using namespace governikus;

defineSingleton(Randomizer)


Randomizer::Randomizer() = default;


QRandomGenerator* Randomizer::getGenerator(bool pSystem)
{
	return pSystem ? QRandomGenerator::system() : QRandomGenerator::global();
}


QByteArray Randomizer::createBytes(int pCount)
{
	std::uniform_int_distribution<quint32> uni;

	auto* generator = getGenerator();
	QByteArray randomBytes;
	while (randomBytes.size() < pCount)
	{
		std::array<char, sizeof(quint32)> number;
		qToBigEndian<quint32>(uni(*generator), number.data());
		randomBytes.append(number.data(), number.size());
	}
	return randomBytes.first(pCount);
}


QUuid Randomizer::createUuid()
{
	return QUuid::fromRfc4122(createBytes(16));
}
