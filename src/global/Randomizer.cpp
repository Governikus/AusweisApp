/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "Randomizer.h"

#include "SingletonHelper.h"

#include <QByteArray>
#include <QRandomGenerator>
#include <QtEndian>

#include <array>
#include <chrono>
#include <openssl/rand.h>

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


template<typename T> QList<T> Randomizer::getEntropy()
{
	QList<T> entropy;

	entropy += static_cast<T>(std::chrono::system_clock::now().time_since_epoch().count());
	entropy += std::random_device()();
	entropy += QRandomGenerator::securelySeeded().generate();

	if (UniversalBuffer<T> buffer; RAND_bytes(buffer.data, sizeof(buffer.data)))
	{
		entropy += buffer.get();
	}

	entropy += getEntropyWin<T>();
	entropy += getEntropyUnixoid<T>();
	entropy += getEntropyApple<T>();

	return entropy;
}


template<typename T> QList<T> Randomizer::getEntropyWin()
{
	QList<T> entropy;

#if defined(Q_OS_WIN) && !defined(Q_OS_WINRT)
	UniversalBuffer<T> buffer;
	HCRYPTPROV provider = 0;
	if (CryptAcquireContext(&provider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		if (CryptGenRandom(provider, sizeof(buffer.data), buffer.data))
		{
			entropy += buffer.get();
		}

		CryptReleaseContext(provider, 0);
	}
#endif

	return entropy;
}


template<typename T> QList<T> Randomizer::getEntropyUnixoid()
{
	QList<T> entropy;

#ifdef SYS_getrandom
	if (UniversalBuffer<T> buffer; syscall(SYS_getrandom, buffer.data, sizeof(buffer.data), GRND_NONBLOCK))
	{
		entropy += buffer.get();
	}
#elif defined(Q_OS_UNIX)
	// Fallback for unixoid systems without SYS_getrandom (like linux before version 3.17 ).
	// This is mainly relevant for older androids.

	UniversalBuffer<T, char> buffer;
	QFile file(QStringLiteral("/dev/urandom"));
	if (file.open(QIODevice::ReadOnly))
	{
		qint64 bytesToRead = sizeof(buffer.data);
		do
		{
			const qint64 bytesRead = file.read(buffer.data, bytesToRead);
			if (bytesRead < 0)
			{
				qCritical() << "Failed to read" << file.fileName();
				break;
			}
			bytesToRead -= bytesRead;
		}
		while (bytesToRead > 0);

		entropy += buffer.get();
		file.close();
	}
	else
	{
		qCritical() << "Failed to open" << file.fileName();
	}
#endif

	return entropy;
}


template<typename T> QList<T> Randomizer::getEntropyApple()
{
	QList<T> entropy;

#if defined(Q_OS_IOS) || defined(Q_OS_MACOS)
	if (UniversalBuffer<T> buffer; SecRandomCopyBytes(kSecRandomDefault, sizeof(buffer.data), buffer.data) == 0)
	{
		entropy += buffer.get();
	}
#endif

	return entropy;
}


Randomizer::Randomizer()
{
	const auto& entropy = getEntropy<std::mt19937_64::result_type>();
	std::seed_seq seed(entropy.cbegin(), entropy.cend());
	mGenerator.seed(seed);

	// We need to seed pseudo random pool of openssl.
	// yes, OpenSSL is an entropy source, too. But not the only one!
	UniversalBuffer<std::mt19937_64::result_type> buffer;
	buffer.set(mGenerator());
	RAND_seed(buffer.data, sizeof(std::mt19937_64::result_type));

	static const int MINIMUM_ENTROPY_SOURCES = 4;
	mSecureRandom = seed.size() >= MINIMUM_ENTROPY_SOURCES;
}


std::mt19937_64& Randomizer::getGenerator()
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
