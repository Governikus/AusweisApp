/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "VolatileSettings.h"

#include "SingletonHelper.h"

using namespace governikus;

defineSingleton(VolatileSettings)


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
	: mUsedAsSdk(true)
	, mDeveloperMode(false)
	, mHandleInterrupt(cHandleInterruptDefault)
	, mMessages()
{
}


bool VolatileSettings::isUsedAsSDK() const
{
	return mUsedAsSdk;
}


void VolatileSettings::setUsedAsSDK(bool pSdk)
{
	mUsedAsSdk = pSdk;
}


bool VolatileSettings::isDeveloperMode() const
{
	return mDeveloperMode;
}


void VolatileSettings::setDeveloperMode(bool pMode)
{
	mDeveloperMode = pMode;
}


bool VolatileSettings::handleInterrupt() const
{
	return mHandleInterrupt;
}


void VolatileSettings::setHandleInterrupt(bool pScan)
{
	mHandleInterrupt = pScan;
}


void VolatileSettings::setMessages(const VolatileSettings::Messages& pMessages)
{
	mMessages = pMessages;
}


const VolatileSettings::Messages& VolatileSettings::getMessages() const
{
	return mMessages;
}
