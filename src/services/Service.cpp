/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "Service.h"

#include "AppSettings.h"
#include "AppUpdateData.h"
#include "ProviderConfiguration.h"
#include "SingletonHelper.h"

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include "ReaderConfiguration.h"
#endif

#include <QMetaObject>

using namespace governikus;

defineSingleton(Service)

Service::Service()
	: mTimer(this)
	, mUpdateScheduled(true)
	, mExplicitSuccessMessage(true)
{
	connect(&mTimer, &QTimer::timeout, this, &Service::onTimedUpdateTriggered);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppUpdateCheckFinished, this, &Service::onAppUpdateFinished);

	mTimer.setSingleShot(true);
	mTimer.start(mOneDayInMs);
}


Service& Service::getInstance()
{
	return *Instance;
}


void Service::updateConfigurations()
{
	QMetaObject::invokeMethod(this, &Service::doConfigurationsUpdate, Qt::QueuedConnection);
}


void Service::updateApp(bool pIgnoreNextVersionskip)
{
	mExplicitSuccessMessage = pIgnoreNextVersionskip;
	mTimer.start(mOneDayInMs);
	QMetaObject::invokeMethod(this, [ = ] {
				doAppUpdate(pIgnoreNextVersionskip);
			}, Qt::QueuedConnection);
}


void Service::doConfigurationsUpdate()
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	Env::getSingleton<ReaderConfiguration>()->update();
#endif

	Env::getSingleton<ProviderConfiguration>()->update();
}


void Service::doAppUpdate(bool pIgnoreNextVersionskip)
{
	Env::getSingleton<AppUpdater>()->checkAppUpdate(pIgnoreNextVersionskip);
}


void Service::onTimedUpdateTriggered()
{
	mUpdateScheduled = true;
	Q_EMIT fireUpdateScheduled();
}


bool Service::isUpdateScheduled()
{
	return mUpdateScheduled;
}


void Service::runUpdateIfNeeded()
{
	if (mUpdateScheduled)
	{
		mUpdateScheduled = false;
		updateConfigurations();
		if (Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoUpdateCheck())
		{
			updateApp();
		}
	}
}


const AppUpdateData& Service::getUpdateData() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData();
}


void Service::onAppUpdateFinished(bool pUpdateAvailable, const GlobalStatus& pError)
{
	if (pUpdateAvailable || pError.isError() || mExplicitSuccessMessage)
	{
		Q_EMIT fireAppUpdateFinished(pUpdateAvailable, pError);
	}
}
