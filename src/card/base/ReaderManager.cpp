/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include <QLoggingCategory>

using namespace governikus;

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
		mWorker = new ReaderManagerWorker();
		mWorker->moveToThread(&mThread);

		connect(&mThread, &QThread::started, mWorker.data(), &ReaderManagerWorker::onThreadStarted);
		connect(&mThread, &QThread::finished, mWorker.data(), &QObject::deleteLater);
		connect(mWorker.data(), &ReaderManagerWorker::fireInitialized, this, &ReaderManager::fireInitialized);

		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::doRemoveCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderPropertiesUpdated, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardInserted, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRemoved, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRetryCounterChanged, this, &ReaderManager::doUpdateCacheEntry);
		connect(mWorker.data(), &ReaderManagerWorker::firePluginAdded, this, &ReaderManager::doFullUpdateCache);
		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::doFullUpdateCache);

		connect(mWorker.data(), &ReaderManagerWorker::firePluginAdded, this, &ReaderManager::firePluginAdded);
		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::fireStatusChanged);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::fireReaderAdded);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::fireReaderRemoved);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderPropertiesUpdated, this, &ReaderManager::fireReaderPropertiesUpdated);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardInserted, this, &ReaderManager::fireCardInserted);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRemoved, this, &ReaderManager::fireCardRemoved);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRetryCounterChanged, this, &ReaderManager::fireCardRetryCounterChanged);

		connect(this, &ReaderManager::fireReaderAdded, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireReaderRemoved, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireCardInserted, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireCardRemoved, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireReaderPropertiesUpdated, this, &ReaderManager::fireReaderEvent);
	}

	mThread.start();
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
	const auto list = Enum<ReaderManagerPlugInType>::getList();
	for (const auto& plugInType : list)
	{
		startScan(plugInType, pAutoConnect);
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
	const auto list = Enum<ReaderManagerPlugInType>::getList();
	for (const auto& plugInType : list)
	{
		stopScan(plugInType, pError);
	}
}


bool ReaderManager::isScanRunning() const
{
	const QMutexLocker mutexLocker(&mMutex);

	bool running = false;
	if (mWorker)
	{
		QMetaObject::invokeMethod(mWorker.data(), qOverload<>(&ReaderManagerWorker::isScanRunning), Qt::BlockingQueuedConnection, &running);
	}
	return running;
}


bool ReaderManager::isScanRunning(ReaderManagerPlugInType pType) const
{
	const QMutexLocker mutexLocker(&mMutex);

	bool running = false;
	if (mWorker)
	{
		QMetaObject::invokeMethod(mWorker.data(), [this, pType] {
					return mWorker->isScanRunning(pType);
				}, Qt::BlockingQueuedConnection, &running);
	}
	return running;
}


QVector<ReaderManagerPlugInInfo> ReaderManager::getPlugInInfos() const
{
	Q_ASSERT(mThread.isRunning() || mThread.isFinished());
	const QMutexLocker mutexLocker(&mMutex);

	QVector<ReaderManagerPlugInInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::getPlugInInfos, Qt::BlockingQueuedConnection, &list);
	return list;
}


void ReaderManager::doUpdateCacheEntry(const ReaderInfo& pInfo)
{
	qCDebug(card) << "Update cache entry:" << pInfo.getName();
	mReaderInfoCache.insert(pInfo.getName(), pInfo);
}


void ReaderManager::doRemoveCacheEntry(const ReaderInfo& pInfo)
{
	qCDebug(card) << "Remove cache entry:" << pInfo.getName();
	mReaderInfoCache.remove(pInfo.getName());
}


void ReaderManager::doFullUpdateCache(const ReaderManagerPlugInInfo& pInfo)
{
	Q_UNUSED(pInfo)
	const QMutexLocker mutexLocker(&mMutex);

	qCDebug(card) << "Start full update of cache...";

	mReaderInfoCache.clear();

	if (mWorker)
	{
		QVector<ReaderInfo> list;
		QMetaObject::invokeMethod(mWorker.data(), [this] {
					return mWorker->getReaderInfos();
				}, Qt::BlockingQueuedConnection, &list);

		for (const auto& info : qAsConst(list))
		{
			qCDebug(card) << "Update cache entry:" << info.getName();
			mReaderInfoCache.insert(info.getName(), info);
		}
	}
}


QVector<ReaderInfo> ReaderManager::getReaderInfos(const ReaderFilter& pFilter) const
{
	Q_ASSERT(mThread.isRunning() || mThread.isFinished());
	const QMutexLocker mutexLocker(&mMutex);

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
	const auto& list = mReaderInfoCache.values();
#else
	const auto& list = mReaderInfoCache.values().toVector(); // clazy:exclude=container-anti-pattern
#endif

	return pFilter.apply(list);
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


void ReaderManager::updateRetryCounters()
{
	Q_ASSERT(mWorker);
	const QMutexLocker mutexLocker(&mMutex);

	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::updateRetryCounters, Qt::QueuedConnection);
}
