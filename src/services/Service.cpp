/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "Service.h"

#include "AppSettings.h"
#include "AppUpdateData.h"
#include "ProviderConfiguration.h"

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#include "ReaderConfiguration.h"
#endif

#include <QMetaObject>

using namespace governikus;


void Service::doAppUpdate(UpdateType pType, bool pForceUpdate)
{
	switch (pType)
	{
		case UpdateType::APPCAST:
			mExplicitSuccessMessage = pForceUpdate;
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
			mTimer.start(mOneDayInMs);
			if (pForceUpdate || Env::getSingleton<AppSettings>()->getGeneralSettings().isAutoUpdateCheck())
			{
				Env::getSingleton<AppUpdater>()->checkAppUpdate(pForceUpdate);
				break;
			}
#endif
			Q_FALLTHROUGH();

		case UpdateType::PROVIDER:
			Env::getSingleton<ProviderConfiguration>()->update();
			break;

		case UpdateType::READER:
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
			Env::getSingleton<ReaderConfiguration>()->update();
#endif
			break;
	}
}


void Service::onTimedUpdateTriggered()
{
	mUpdateScheduled = true;
	Q_EMIT fireUpdateScheduled();
}


void Service::onProviderUpdateFinished()
{
	doAppUpdate(UpdateType::READER);
}


void Service::onAppcastFinished(bool pUpdateAvailable, const GlobalStatus& pError)
{
	if (pUpdateAvailable || pError.isError() || mExplicitSuccessMessage)
	{
		Q_EMIT fireAppcastFinished(pUpdateAvailable, pError);
	}

	if (pError.isNoError() || pError.getStatusCode() == GlobalStatus::Code::Downloader_Missing_Platform)
	{
		doAppUpdate(UpdateType::PROVIDER);
	}
}


Service::Service()
	: mTimer(this)
	, mUpdateScheduled(true)
	, mExplicitSuccessMessage(true)
{
	connect(&mTimer, &QTimer::timeout, this, &Service::onTimedUpdateTriggered);
	connect(Env::getSingleton<ProviderConfiguration>(), &ProviderConfiguration::fireUpdated, this, &Service::onProviderUpdateFinished);
	connect(Env::getSingleton<ProviderConfiguration>(), &ProviderConfiguration::fireNoUpdateAvailable, this, &Service::onProviderUpdateFinished);
	connect(Env::getSingleton<AppUpdater>(), &AppUpdater::fireAppcastCheckFinished, this, &Service::onAppcastFinished);

	mTimer.setSingleShot(true);
	mTimer.start(mOneDayInMs);
}


void Service::updateAppcast()
{
	doAppUpdate(UpdateType::APPCAST, true);
}


bool Service::isUpdateScheduled() const
{
	return mUpdateScheduled;
}


void Service::runUpdateIfNeeded()
{
	if (mUpdateScheduled)
	{
		mUpdateScheduled = false;
		QMetaObject::invokeMethod(this, [this] {
					doAppUpdate(UpdateType::APPCAST);
				}, Qt::QueuedConnection);
	}
}


const AppUpdateData& Service::getUpdateData() const
{
	return Env::getSingleton<AppUpdater>()->getUpdateData();
}
