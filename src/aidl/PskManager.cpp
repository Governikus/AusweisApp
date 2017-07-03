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


PskManager & PskManager::getInstance()
{
	return *instance;
}


QString PskManager::generatePsk(const QString& pClientPartialPsk)
{
	const static int TIMESTAMP_BYTE_COUNT = 64 / 8;
	const static int RANDOM_BYTE_COUNT = 256;

	QMutexLocker locker(&mPskMutex);

	QByteArray timeStampBytes;
	timeStampBytes.reserve(TIMESTAMP_BYTE_COUNT);
	qint64 timeStamp = QDateTime::currentMSecsSinceEpoch();
	for (int i = 0; i < TIMESTAMP_BYTE_COUNT; i++)
	{
		timeStampBytes += timeStamp & 0xFF;
		timeStamp >>= 8;
	}

	QByteArray randomBytes;
	randomBytes.reserve(RANDOM_BYTE_COUNT);

	Randomizer& randomizer = Randomizer::getInstance();
	mSecureRandomPsk = randomizer.isSecureRandom();
	std::mt19937& generator = randomizer.getGenerator();
	for (int i = 0; i < RANDOM_BYTE_COUNT; i += 4)
	{
		const qint32 randomNumber = generator();
		randomBytes += (char) randomNumber & 0xFF;
		randomBytes += (char) (randomNumber >> 8) & 0xFF;
		randomBytes += (char) (randomNumber >> 16) & 0xFF;
		randomBytes += (char) (randomNumber >> 24) & 0xFF;
	}

	QByteArray mServerInputBytes;
	mServerInputBytes.reserve(TIMESTAMP_BYTE_COUNT + RANDOM_BYTE_COUNT);
	mServerInputBytes += timeStampBytes;
	mServerInputBytes += randomBytes;

	QString clientPartialPsk = pClientPartialPsk.trimmed();
	if (clientPartialPsk.startsWith(QLatin1String("0x")))
	{
		clientPartialPsk = clientPartialPsk.mid(2, -1);
	}
	const QByteArray clientInputBytes = QByteArray::fromHex(clientPartialPsk.toLatin1());

	QCryptographicHash hashFunction(QCryptographicHash::Sha256);
	hashFunction.addData(mServerInputBytes);
	hashFunction.addData(clientInputBytes);
	mPsk = hashFunction.result();

	return QString::fromLatin1(mPsk.toHex());
}


QByteArray PskManager::getPsk()
{
	QMutexLocker locker(&mPskMutex);
	return mPsk;
}


bool PskManager::isSecureRandomPsk()
{
	QMutexLocker locker(&mPskMutex);
	return mSecureRandomPsk;
}
