/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QRandomGenerator>
#include <QUuid>

class test_Randomizer;


namespace governikus
{

class Randomizer
{
	Q_DISABLE_COPY(Randomizer)
	friend class ::test_Randomizer;

	protected:
		Randomizer();
		~Randomizer() = default;

	public:
		static Randomizer& getInstance();

		[[nodiscard]] QRandomGenerator* getGenerator(bool pSystem = true);

		[[nodiscard]] QByteArray createBytes(int pCount);
		[[nodiscard]] QUuid createUuid();

};

} // namespace governikus
