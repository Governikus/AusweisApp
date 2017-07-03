#include "Randomizer.h"

#include "SingletonHelper.h"

#include <chrono>
#include <openssl/rand.h>

#ifdef Q_OS_WIN
	#include <windows.h>
#elif defined(Q_OS_UNIX)
	#if defined(Q_OS_Linux) && !defined(Q_OS_ANDROID)
		#include <syscall.h>
	#endif
	#if defined(SYS_getrandom)
		#include <linux/random.h>
		#include <unistd.h>
	#else
		#include <QDebug>
		#include <QFile>
	#endif
	#if defined(Q_OS_IOS) || defined(Q_OS_OSX)
		#include <Security/SecRandom.h>
	#endif
#endif


using namespace governikus;

defineSingleton(Randomizer)


template<typename T, typename U = uchar> union UniversalBuffer
{
	T number;
	U data[sizeof(T)] = {};
};

template<typename T> QList<T> Randomizer::getEntropy()
{
	QList<T> entropy;

	entropy += static_cast<T>(std::chrono::system_clock::now().time_since_epoch().count());
	entropy += std::random_device()();
	entropy += static_cast<T>(qrand());

	UniversalBuffer<T> buffer;
	if (RAND_bytes(buffer.data, sizeof(buffer.data)))
	{
		entropy += buffer.number;
	}

	entropy += getEntropyWin<T>();
	entropy += getEntropyUnixoid<T>();
	entropy += getEntropyApple<T>();

	return entropy;
}


template<typename T> QList<T> Randomizer::getEntropyWin()
{
	QList<T> entropy;

#ifdef Q_OS_WIN32
	UniversalBuffer<T> buffer;
	HCRYPTPROV provider = 0;
	if (CryptAcquireContext(&provider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
	{
		if (CryptGenRandom(provider, sizeof(buffer.data), buffer.data))
		{
			entropy += buffer.number;
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
	UniversalBuffer<T> buffer;
	if (syscall(SYS_getrandom, buffer.data, sizeof(buffer.data), GRND_NONBLOCK))
	{
		entropy += buffer.number;
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

		entropy += buffer.number;
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

#if defined(Q_OS_IOS) || defined(Q_OS_OSX)
	UniversalBuffer<T> buffer;
	if (SecRandomCopyBytes(kSecRandomDefault, sizeof(buffer.data), buffer.data) == 0)
	{
		entropy += buffer.number;
	}
#endif

	return entropy;
}


Randomizer::Randomizer()
{
	const auto& entropy = getEntropy<std::mt19937::result_type>();
	std::seed_seq seed(entropy.cbegin(), entropy.cend());
	mGenerator.seed(seed);

	static const int MINUMUM_ENTROPY_SOURCES = 5;
	mSecureRandom = seed.size() >= MINUMUM_ENTROPY_SOURCES;
}


Randomizer::~Randomizer()
{
}


Randomizer& Randomizer::getInstance()
{
	return *Instance;
}


std::mt19937& Randomizer::getGenerator()
{
	return mGenerator;
}


bool Randomizer::isSecureRandom() const
{
	return mSecureRandom;
}
