/**
 * Copyright (c) 2026 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdManager.h"

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#include "ApplicationModel.h"
#endif
#include "ReaderManager.h"
#include "RemoteIfdClient.h"


using namespace governikus;


RemoteIfdManager::RemoteIfdManager(std::function<bool()> pCondition, bool pDetectionOnly)
	: QObject()
	, mCondition(std::move(pCondition))
	, mDetectionOnly(pDetectionOnly)
	, mRemoteIfdWasRunning(false)
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	connect(Env::getSingleton<ApplicationModel>(), &ApplicationModel::fireApplicationStateChanged,
			this, &RemoteIfdManager::onApplicationStateChanged);
#endif
}


void RemoteIfdManager::onApplicationStateChanged(bool pIsAppInForeground)
{
	if (!mCondition())
	{
		return;
	}

	if (pIsAppInForeground)
	{
		if (!mRemoteIfdWasRunning)
		{
			return;
		}

		mRemoteIfdWasRunning = false;
		if (mDetectionOnly)
		{
			Env::getSingleton<RemoteIfdClient>()->startDetection();
			return;
		}

		Env::getSingleton<ReaderManager>()->startScan(ReaderManagerPluginType::REMOTE_IFD);
		return;
	}

	if (mDetectionOnly)
	{
		if (Env::getSingleton<RemoteIfdClient>()->isDetecting())
		{
			mRemoteIfdWasRunning = true;
			Env::getSingleton<RemoteIfdClient>()->stopDetection();
		}

		return;
	}

	if (Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::REMOTE_IFD).isScanRunning())
	{
		mRemoteIfdWasRunning = true;
		Env::getSingleton<ReaderManager>()->stopScan(ReaderManagerPluginType::REMOTE_IFD);
	}

}
