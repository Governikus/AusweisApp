/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include <QLoggingCategory>


using namespace governikus;


QList<std::function<void()>> ReaderManager::cMainThreadInit;


Q_DECLARE_LOGGING_CATEGORY(card)


ReaderManager::ReaderManager()
	: QObject()
	, mMutex()
	, mThread()
	, mWorker()
{
	mThread.setObjectName(QStringLiteral("ReaderManagerThread"));
}


ReaderManager::~ReaderManager()
{
	{
		const QMutexLocker mutexLocker(&mMutex);
		if (!mThread.isRunning())
		{
			return;
		}
	}

	qCWarning(card) << "ReaderManager is not stopped correctly...";
	shutdown();
}


void ReaderManager::addMainThreadInit(const std::function<void()>& pFunc)
{
	cMainThreadInit << pFunc;
}


void ReaderManager::init()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (mThread.isRunning())
	{
		qCWarning(card) << "ReaderManager already initialized";
		return;
	}

	if (mWorker.isNull())
	{
		for (const auto& func : std::as_const(cMainThreadInit))
		{
			func();
		}
		cMainThreadInit.clear();

		mWorker = new ReaderManagerWorker();
		mWorker->moveToThread(&mThread);

		connect(&mThread, &QThread::started, mWorker.data(), &ReaderManagerWorker::onThreadStarted);
		connect(&mThread, &QThread::finished, mWorker.data(), &QObject::deleteLater);
		connect(mWorker.data(), &ReaderManagerWorker::fireInitialized, this, &ReaderManager::fireInitialized);

		connect(mWorker.data(), &ReaderManagerWorker::firePluginAdded, this, &ReaderManager::doUpdatePluginCache);
		connect(mWorker.data(), &ReaderManagerWorker::firePluginAdded, this, &ReaderManager::firePluginAdded);

		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::doUpdatePluginCache);
		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::fireStatusChanged);

		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::fireReaderAdded);

		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::doRemoveCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::fireReaderRemoved);

		connect(mWorker.data(), &ReaderManagerWorker::fireReaderPropertiesUpdated, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderPropertiesUpdated, this, &ReaderManager::fireReaderPropertiesUpdated);

		connect(mWorker.data(), &ReaderManagerWorker::fireCardInserted, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardInserted, this, &ReaderManager::fireCardInserted);

		connect(mWorker.data(), &ReaderManagerWorker::fireCardRemoved, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRemoved, this, &ReaderManager::fireCardRemoved);

		connect(mWorker.data(), &ReaderManagerWorker::fireCardInfoChanged, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardInfoChanged, this, &ReaderManager::fireCardInfoChanged);
	}

	mThread.start();
}


void ReaderManager::reset(ReaderManagerPlugInType pType)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot reset a plugin if ReaderManager-Thread is not active";
		return;
	}

	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this, pType] {
			mWorker->reset(pType);
		}, Qt::QueuedConnection);
}


void ReaderManager::insert(const ReaderInfo& pReaderInfo, const QVariant& pData)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot insert a card if ReaderManager-Thread is not active";
		return;
	}

	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this, pReaderInfo, pData] {
			mWorker->insert(pReaderInfo, pData);
		}, Qt::QueuedConnection);
}


void ReaderManager::shelve()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot shelve cards if ReaderManager-Thread is not active";
		return;
	}

	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this] {
			mWorker->shelve();
		}, Qt::QueuedConnection);
}


void ReaderManager::shutdown()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (mThread.isRunning() && !mThread.isInterruptionRequested())
	{
		qCDebug(card) << "Shutdown ReaderManager...";
		mThread.requestInterruption(); // do not try to stop AGAIN from dtor
		if (mWorker)
		{
			QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::shutdown, Qt::BlockingQueuedConnection);
		}
		mThread.quit();
		mThread.wait(5000);
		mReaderInfoCache.clear();
		mPlugInInfoCache.clear();
		qCDebug(card).noquote() << mThread.objectName() << "stopped:" << !mThread.isRunning();
	}
}


void ReaderManager::startScan(ReaderManagerPlugInType pType, bool pAutoConnect)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot start scan if ReaderManager-Thread is not active";
		return;
	}

	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this, pType, pAutoConnect] {
			mWorker->startScan(pType, pAutoConnect);
		}, Qt::QueuedConnection);
}


void ReaderManager::startScanAll(bool pAutoConnect)
{
	for (const auto& entry : std::as_const(mPlugInInfoCache))
	{
		startScan(entry.getPlugInType(), pAutoConnect);
	}
}


void ReaderManager::stopScan(ReaderManagerPlugInType pType, const QString& pError)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot stop scan if ReaderManager-Thread is not active";
		return;
	}

	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this, pType, pError] {
			mWorker->stopScan(pType, pError);
		}, Qt::QueuedConnection);
}


void ReaderManager::stopScanAll(const QString& pError)
{
	for (const auto& entry : std::as_const(mPlugInInfoCache))
	{
		stopScan(entry.getPlugInType(), pError);
	}
}


ReaderManagerPlugInInfo ReaderManager::getPlugInInfo(ReaderManagerPlugInType pType) const
{
	const QMutexLocker mutexLocker(&mMutex);

	return mPlugInInfoCache.value(pType, ReaderManagerPlugInInfo(pType));
}


void ReaderManager::doUpdateCacheEntry(const ReaderInfo& pInfo)
{
	const QMutexLocker mutexLocker(&mMutex);

	qCDebug(card).noquote() << "Update cache entry:" << pInfo.getName();
	mReaderInfoCache.insert(pInfo.getName(), pInfo);
}


void ReaderManager::doRemoveCacheEntry(const ReaderInfo& pInfo)
{
	const QMutexLocker mutexLocker(&mMutex);

	qCDebug(card).noquote() << "Remove cache entry:" << pInfo.getName();
	mReaderInfoCache.remove(pInfo.getName());
}


void ReaderManager::doUpdatePluginCache(const ReaderManagerPlugInInfo& pInfo)
{
	const QMutexLocker mutexLocker(&mMutex);

	qCDebug(card).noquote() << "Update cache entry:" << pInfo.getPlugInType();
	mPlugInInfoCache.insert(pInfo.getPlugInType(), pInfo);
}


QList<ReaderInfo> ReaderManager::getReaderInfos(const ReaderFilter& pFilter) const
{
	Q_ASSERT(mThread.isRunning() || mThread.isFinished());
	const QMutexLocker mutexLocker(&mMutex);
	return pFilter.apply(mReaderInfoCache.values());
}


ReaderInfo ReaderManager::getReaderInfo(const QString& pReaderName) const
{
	Q_ASSERT(mThread.isRunning() || mThread.isFinished());
	const QMutexLocker mutexLocker(&mMutex);

	const ReaderInfo info = mReaderInfoCache.value(pReaderName);
	return info.getName().isEmpty() ? ReaderInfo(pReaderName) : info;
}


void ReaderManager::updateReaderInfo(const QString& pReaderName)
{
	Q_ASSERT(mWorker);
	QMetaObject::invokeMethod(mWorker.data(), [this, pReaderName] {
			mWorker->updateReaderInfo(pReaderName);
		}, Qt::BlockingQueuedConnection); // needed to force the ReaderInfo update, else StateMachine loops based on stale state can occur
}
