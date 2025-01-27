/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Helper to get a Randomizer.
 */

#pragma once

#include <QUuid>

#include <limits>
#include <openssl/rand.h>
#include <random>
#include <memory>

class test_Randomizer;

namespace governikus
{

class OpenSSLGenerator {
public:
	using result_type = uint64_t;

	static constexpr result_type min() {
		return std::numeric_limits<result_type>::min();
	}

	static constexpr result_type max() {
		return std::numeric_limits<result_type>::max();
	}

	result_type operator()();
};

class Randomizer
{
	Q_DISABLE_COPY(Randomizer)
	friend class ::test_Randomizer;

	private:
		OpenSSLGenerator mGenerator;
		bool mSecureRandom;

		[[nodiscard]] static size_t getEntropy(std::shared_ptr<EVP_MD_CTX> mdCtx);
		[[nodiscard]] static bool getEntropyWin(std::shared_ptr<EVP_MD_CTX> mdCtx);
		[[nodiscard]] static bool getEntropyUnixoid(std::shared_ptr<EVP_MD_CTX> mdCtx);
		[[nodiscard]] static bool getEntropyApple(std::shared_ptr<EVP_MD_CTX> mdCtx);

	protected:
		Randomizer();
		~Randomizer() = default;

	public:
		static Randomizer& getInstance();

		[[nodiscard]] OpenSSLGenerator& getGenerator();
		[[nodiscard]] bool isSecureRandom() const;

		[[nodiscard]] QUuid createUuid();

};

} // namespace governikus
