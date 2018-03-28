/*!
 * \brief Helper to get a Randomizer.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QList>
#include <random>

namespace governikus
{

class Randomizer
{
	private:
		std::mt19937 mGenerator;
		bool mSecureRandom;

		template<typename T> static QList<T> getEntropy();
		template<typename T> static QList<T> getEntropyWin();
		template<typename T> static QList<T> getEntropyUnixoid();
		template<typename T> static QList<T> getEntropyApple();

		Randomizer(const Randomizer&) = delete;
		Randomizer& operator=(const Randomizer&) = delete;

	protected:
		Randomizer();
		~Randomizer();

	public:
		static Randomizer& getInstance();

		std::mt19937& getGenerator();
		bool isSecureRandom() const;

};

} /* namespace governikus */
