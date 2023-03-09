/**
 * Copyright (c) 2020-2023 Governikus GmbH & Co. KG, Germany
 */

#include "VolatileSettings.h"

#include <QReadLocker>
#include <QWriteLocker>

using namespace governikus;

VolatileSettings::Messages::Messages(const QString& pSessionStarted,
		const QString& pSessionFailed,
		const QString& pSessionSucceeded,
		const QString& pSessionInProgress)
	: mSessionStarted(pSessionStarted)
	, mSessionFailed(pSessionFailed.isNull() ? QLatin1String("") : pSessionFailed) // Null string is interpreted as 'success'
	, mSessionSucceeded(pSessionSucceeded)
	, mSessionInProgress(pSessionInProgress)
{
}


QString VolatileSettings::Messages::getSessionStarted() const
{
	return mSessionStarted;
}


QString VolatileSettings::Messages::getSessionFailed() const
{
	return mSessionFailed;
}


QString VolatileSettings::Messages::getSessionSucceeded() const
{
	return mSessionSucceeded;
}


QString VolatileSettings::Messages::getSessionInProgress() const
{
	return mSessionInProgress;
}


VolatileSettings::VolatileSettings()
	: mLock()
	, mUsedAsSdk(true)
	, mDeveloperMode(false)
	, mHandleInterrupt(cHandleInterruptDefault)
	, mMessages()
	, mDelay(0)
{
}


bool VolatileSettings::isUsedAsSDK() const
{
	const QReadLocker locker(&mLock);
	return mUsedAsSdk;
}


void VolatileSettings::setUsedAsSDK(bool pSdk)
{
	QWriteLocker locker(&mLock);

	if (mUsedAsSdk != pSdk)
	{
		mUsedAsSdk = pSdk;
		locker.unlock();
		Q_EMIT fireUsedAsSdkChanged();
	}
}


bool VolatileSettings::isDeveloperMode() const
{
	const QReadLocker locker(&mLock);
	return mDeveloperMode;
}


void VolatileSettings::setDeveloperMode(bool pMode)
{
	const QWriteLocker locker(&mLock);
	mDeveloperMode = pMode;
}


bool VolatileSettings::handleInterrupt() const
{
	const QReadLocker locker(&mLock);
	return mHandleInterrupt;
}


void VolatileSettings::setHandleInterrupt(bool pScan)
{
	const QWriteLocker locker(&mLock);
	mHandleInterrupt = pScan;
}


void VolatileSettings::setMessages(const VolatileSettings::Messages& pMessages)
{
	const QWriteLocker locker(&mLock);
	mMessages = pMessages;
}


VolatileSettings::Messages VolatileSettings::getMessages() const
{
	const QReadLocker locker(&mLock);
	return mMessages;
}


void VolatileSettings::setDelay(ulong pDelay)
{
	const QWriteLocker locker(&mLock);
	mDelay = pDelay;
}


ulong VolatileSettings::getDelay() const
{
	const QReadLocker locker(&mLock);
	return mDelay;
}
