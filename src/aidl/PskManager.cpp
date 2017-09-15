/*!
 * PskManager.cpp
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "PskManager.h"
#include "Randomizer.h"

#include <QCryptographicHash>
#include <QDateTime>
#include <QGlobalStatic>
#include <QMutexLocker>


using namespace governikus;

Q_GLOBAL_STATIC(PskManager, instance)


PskManager::PskManager()
	: mPsk()
	, mSecureRandomPsk(false)
	, mPskMutex()
{
}


PskManager& PskManager::getInstance()
{
	return *instance;
}


QByteArray PskManager::generatePsk()
{
	const static int TIMESTAMP_BYTE_COUNT = 64 / 8;
	const static int RANDOM_BYTE_COUNT = 256;

	const QMutexLocker locker(&mPskMutex);

	QByteArray timeStampBytes;
	timeStampBytes.reserve(TIMESTAMP_BYTE_COUNT);
	qint64 timeStamp = QDateTime::currentMSecsSinceEpoch();
	for (int i = 0; i < TIMESTAMP_BYTE_COUNT; i++)
	{
		timeStampBytes += static_cast<char>(timeStamp & 0xFF);
		timeStamp >>= 8;
	}

	QByteArray randomBytes;
	randomBytes.reserve(RANDOM_BYTE_COUNT);

	Randomizer& randomizer = Randomizer::getInstance();
	mSecureRandomPsk = randomizer.isSecureRandom();
	std::mt19937& generator = randomizer.getGenerator();
	for (int i = 0; i < RANDOM_BYTE_COUNT; i += 4)
	{
		const auto randomNumber = generator();
		randomBytes += static_cast<char>(randomNumber & 0xFF);
		randomBytes += static_cast<char>((randomNumber >> 8) & 0xFF);
		randomBytes += static_cast<char>((randomNumber >> 16) & 0xFF);
		randomBytes += static_cast<char>((randomNumber >> 24) & 0xFF);
	}

	QByteArray mServerInputBytes;
	mServerInputBytes.reserve(TIMESTAMP_BYTE_COUNT + RANDOM_BYTE_COUNT);
	mServerInputBytes += timeStampBytes;
	mServerInputBytes += randomBytes;

	QCryptographicHash hashFunction(QCryptographicHash::Sha256);
	hashFunction.addData(mServerInputBytes);
	mPsk = hashFunction.result().toHex();
	return mPsk;
}


QByteArray PskManager::getPsk()
{
	const QMutexLocker locker(&mPskMutex);
	return mPsk;
}


bool PskManager::isSecureRandomPsk()
{
	const QMutexLocker locker(&mPskMutex);
	return mSecureRandomPsk;
}
