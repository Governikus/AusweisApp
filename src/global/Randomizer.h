/*!
 * \brief Helper to get a Randomizer.
 *
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QList>
#include <QUuid>

#include <random>

class test_Randomizer;

namespace governikus
{

class Randomizer
{
	Q_DISABLE_COPY(Randomizer)
	friend class ::test_Randomizer;

	private:
		template<typename T = std::mt19937_64::result_type, typename U = uchar> struct UniversalBuffer
		{
			U data[sizeof(T)] = {};

			T get()
			{
#if __cpp_lib_bit_cast >= 201806
				return std::bit_cast<T>(data);

#else
				T number;
				memcpy(&number, &data, sizeof(T));
				return number;

#endif
			}


			void set(T pNumber)
			{
				memcpy(&data, &pNumber, sizeof(T));
			}


			static_assert(sizeof(T) == sizeof(data));
		};

		std::mt19937_64 mGenerator;
		bool mSecureRandom;

		template<typename T> static QList<T> getEntropy();
		template<typename T> static QList<T> getEntropyWin();
		template<typename T> static QList<T> getEntropyUnixoid();
		template<typename T> static QList<T> getEntropyApple();

	protected:
		Randomizer();
		~Randomizer() = default;

	public:
		static Randomizer& getInstance();

		[[nodiscard]] std::mt19937_64& getGenerator();
		[[nodiscard]] bool isSecureRandom() const;

		[[nodiscard]] QUuid createUuid();

};

} // namespace governikus
