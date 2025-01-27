/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Randomizer.h"

#include "SingletonHelper.h"

#include <QByteArray>
#include <QRandomGenerator>
#include <QtEndian>

#include <chrono>
#include <cstdint>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <random>

#ifdef Q_OS_WIN
	#include <windows.h>
#elif defined(Q_OS_UNIX)
	#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
		#include <syscall.h>
	#endif
	#if defined(SYS_getrandom)
		#include <linux/random.h>
		#include <unistd.h>
	#else
		#include <QDebug>
		#include <QFile>
	#endif
	#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
		#include <Security/SecRandom.h>
	#endif
#endif


using namespace governikus;

defineSingleton(Randomizer)

const size_t ENTROPY_FETCH_SIZE = 256 / 8;

OpenSSLGenerator::result_type OpenSSLGenerator::operator()() {
	result_type buffer{0};

	int ret = RAND_bytes(reinterpret_cast<unsigned char*>(&buffer), sizeof(buffer));
	Q_ASSERT(ret == 1);

	return buffer;
}


size_t Randomizer::getEntropy(std::shared_ptr<EVP_MD_CTX> mdCtx)
{
	int ret;
	size_t numSources = 3;

	auto clockTicks = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	ret = EVP_DigestUpdate(mdCtx.get(), &clockTicks, sizeof(clockTicks));
	Q_ASSERT(ret == 1);

	for (size_t i = 0; i < ENTROPY_FETCH_SIZE / sizeof(std::random_device::result_type); ++i) {
		auto randomDeviceRand = std::random_device()();
		ret = EVP_DigestUpdate(mdCtx.get(), &randomDeviceRand, sizeof(randomDeviceRand));
		Q_ASSERT(ret == 1);
		OPENSSL_cleanse(&randomDeviceRand, sizeof(randomDeviceRand));
	}

	quint64 qtBuffer[ENTROPY_FETCH_SIZE / sizeof(quint64)];
	QRandomGenerator::securelySeeded().fillRange(qtBuffer);
	ret = EVP_DigestUpdate(mdCtx.get(), &qtBuffer, ENTROPY_FETCH_SIZE);
	Q_ASSERT(ret == 1);
	OPENSSL_cleanse(&qtBuffer, ENTROPY_FETCH_SIZE);

	if (unsigned char buffer[ENTROPY_FETCH_SIZE]; RAND_bytes(buffer, sizeof(buffer)))
	{
		ret = EVP_DigestUpdate(mdCtx.get(), buffer, sizeof(buffer));
		Q_ASSERT(ret == 1);
		++numSources;
		OPENSSL_cleanse(buffer, sizeof(buffer));
	}

	if (getEntropyWin(mdCtx))
		++numSources;
	if (getEntropyUnixoid(mdCtx))
		++numSources;
	if (getEntropyApple(mdCtx))
		++numSources;

	return numSources;
}


bool Randomizer::getEntropyWin(std::shared_ptr<EVP_MD_CTX> mdCtx)
{
	bool hasEntropy = false;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
	unsigned char buffer[ENTROPY_FETCH_SIZE];
	HCRYPTPROV provider = 0;
	if (CryptAcquireContext(&provider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		if (CryptGenRandom(provider, sizeof(buffer), buffer))
		{
			int ret = EVP_DigestUpdate(mdCtx.get(), buffer, sizeof(buffer));
			Q_ASSERT(ret == 1);
			hasEntropy = true;
		}

		CryptReleaseContext(provider, 0);
	}
	OPENSSL_cleanse(buffer, sizeof(buffer));
#else
	Q_UNUSED(mdCtx)
#endif

	return hasEntropy;
}


bool Randomizer::getEntropyUnixoid(std::shared_ptr<EVP_MD_CTX> mdCtx)
{
	bool hasEntropy = false;

#ifdef SYS_getrandom
	{
		unsigned char buffer[ENTROPY_FETCH_SIZE];
		long int sys_ret;
		ssize_t bytesToRead = ENTROPY_FETCH_SIZE;
		while (bytesToRead > 0) {
			sys_ret = syscall(SYS_getrandom, buffer + ENTROPY_FETCH_SIZE - bytesToRead, bytesToRead, GRND_NONBLOCK);
			if (sys_ret > 0 && sys_ret <= static_cast<long int>(bytesToRead)) {
				bytesToRead -= sys_ret;
			}
			if (sys_ret == -1 && !(errno == EINTR || errno == EAGAIN)) {
				// return from loop, if something goes wrong unexpectedly
				break;
			}
		};
		Q_ASSERT(bytesToRead == 0);

		int ret = EVP_DigestUpdate(mdCtx.get(), buffer, sizeof(buffer));
		Q_ASSERT(ret == 1);

		hasEntropy = true;

		OPENSSL_cleanse(buffer, sizeof(buffer));
	}
#elif defined(Q_OS_UNIX)
	// Fallback for unixoid systems without SYS_getrandom (like linux before version 3.17 ).
	// This is mainly relevant for older androids.

	QFile file(QStringLiteral("/dev/urandom"));
	if (file.open(QIODevice::ReadOnly))
	{
		qint64 bytesToRead = sizeof(buffer);
		do
		{
			const qint64 bytesRead = file.read(buffer + sizeof(buffer) - bytesToRead, bytesToRead);
			if (bytesRead < 0)
			{
				qCritical() << "Failed to read" << file.fileName();
				break;
			}
			bytesToRead -= bytesRead;
		}
		while (bytesToRead > 0);

		int ret = EVP_DigestUpdate(mdCtx.get(), buffer, sizeof(buffer));
		Q_ASSERT(ret == 1);
		hasEntropy = bytesToRead == 0;
		file.close();

		OPENSSL_cleanse(buffer, sizeof(buffer));
	}
	else
	{
		qCritical() << "Failed to open" << file.fileName();
	}
#else
	Q_UNUSED(mdCtx)
#endif

	return hasEntropy;
}


bool Randomizer::getEntropyApple(std::shared_ptr<EVP_MD_CTX> mdCtx)
{
	bool hasEntropy = false;

#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
	if (unsigned char buffer[ENTROPY_FETCH_SIZE]; SecRandomCopyBytes(kSecRandomDefault, sizeof(buffer), buffer) == 0)
	{
		int ret = EVP_DigestUpdate(mdCtx.get(), buffer, sizeof(buffer));
		Q_ASSERT(ret == 1);
		hasEntropy = true;
		OPENSSL_cleanse(buffer, sizeof(buffer));
	}
#else
	Q_UNUSED(mdCtx)
#endif

	return hasEntropy;
}


Randomizer::Randomizer()
{
	auto mdCtx = std::shared_ptr<EVP_MD_CTX>(EVP_MD_CTX_new(), EVP_MD_CTX_free);

	int ret = EVP_DigestInit_ex(mdCtx.get(), EVP_sha512(), nullptr);
	Q_ASSERT(ret == 1);

	size_t numSources = getEntropy(mdCtx);

	const size_t SHA512_DIGEST_SIZE = 512 / 8;
	unsigned char seedBuffer[SHA512_DIGEST_SIZE];
	unsigned int mdLen = SHA512_DIGEST_SIZE;

	ret = EVP_DigestFinal_ex(mdCtx.get(), seedBuffer, &mdLen);
	Q_ASSERT(ret == 1);
	Q_ASSERT(mdLen = SHA512_DIGEST_SIZE);

	/*
	* OpenSSL automatically seeds itself, unless configured otherwise.
	* This is just defensive programming.
	*/
	RAND_seed(seedBuffer, sizeof(seedBuffer));
	OPENSSL_cleanse(seedBuffer, sizeof(seedBuffer));

	static const int MINIMUM_ENTROPY_SOURCES = 4;
	mSecureRandom = numSources >= MINIMUM_ENTROPY_SOURCES;
}


OpenSSLGenerator& Randomizer::getGenerator()
{
	return mGenerator;
}


bool Randomizer::isSecureRandom() const
{
	return mSecureRandom;
}


QUuid Randomizer::createUuid()
{
	std::uniform_int_distribution<quint32> uni;

	QByteArray randomBytes;
	while (randomBytes.size() < 16)
	{
		std::array<char, sizeof(quint32)> number;
		qToBigEndian<quint32>(uni(mGenerator), number.data());
		randomBytes.append(number.data(), number.size());
	}
	return QUuid::fromRfc4122(randomBytes);
}
