/*!
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "SingletonHelper.h"

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(ReaderManager)

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


ReaderManager& ReaderManager::getInstance()
{
	return *Instance;
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

		connect(mWorker.data(), &ReaderManagerWorker::firePluginAdded, this, &ReaderManager::firePluginAdded);
		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::fireStatusChanged);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::fireReaderAdded);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::fireReaderRemoved);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderDeviceError, this, &ReaderManager::fireReaderDeviceError);
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
		mThread.quit();
		mThread.wait(5000);
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

	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				mWorker->startScan(pType, pAutoConnect);
			}, Qt::QueuedConnection);
}


void ReaderManager::startScanAll(bool pAutoConnect)
{
	for (const auto& plugInType : Enum<ReaderManagerPlugInType>::getList())
	{
		startScan(plugInType, pAutoConnect);
	}
}


void ReaderManager::stopScan(ReaderManagerPlugInType pType)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot stop scan if ReaderManager-Thread is not active";
		return;
	}

	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				mWorker->stopScan(pType);
			}, Qt::QueuedConnection);
}


void ReaderManager::stopScanAll()
{
	for (const auto& plugInType : Enum<ReaderManagerPlugInType>::getList())
	{
		stopScan(plugInType);
	}
}


bool ReaderManager::isScanRunning()
{
	bool running = false;
	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::isScanRunning, Qt::BlockingQueuedConnection, &running);
	return running;
}


QVector<ReaderManagerPlugInInfo> ReaderManager::getPlugInInfos() const
{
	const QMutexLocker mutexLocker(&mMutex);

	QVector<ReaderManagerPlugInInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::getPlugInInfos, Qt::BlockingQueuedConnection, &list);
	return list;
}


QVector<ReaderInfo> ReaderManager::getReaderInfos(ReaderManagerPlugInType pType) const
{
	return getReaderInfos(QVector<ReaderManagerPlugInType> {pType});
}


QVector<ReaderInfo> ReaderManager::getReaderInfos(const ReaderFilter& pFilter) const
{
	const QMutexLocker mutexLocker(&mMutex);

	QVector<ReaderInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				return mWorker->getReaderInfos(pFilter);
			}, Qt::BlockingQueuedConnection, &list);
	return pFilter.apply(list);
}


ReaderInfo ReaderManager::getReaderInfo(const QString& pReaderName) const
{
	const QMutexLocker mutexLocker(&mMutex);

	ReaderInfo info(pReaderName);
	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				return mWorker->getReaderInfo(pReaderName);
			}, Qt::BlockingQueuedConnection, &info);
	return info;
}


void ReaderManager::updateReaderInfo(const QString& pReaderName)
{
	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				mWorker->updateReaderInfo(pReaderName);
			}, Qt::BlockingQueuedConnection); // needed to force the ReaderInfo update, else StateMachine loops based on stale state can occur
}


void ReaderManager::connectReader(const QString& pReaderName)
{
	const QMutexLocker mutexLocker(&mMutex);

	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				mWorker->connectReader(pReaderName);
			}, Qt::QueuedConnection);
}


void ReaderManager::disconnectReader(const QString& pReaderName)
{
	const QMutexLocker mutexLocker(&mMutex);

	QMetaObject::invokeMethod(mWorker.data(), [ = ] {
				mWorker->disconnectReader(pReaderName);
			}, Qt::QueuedConnection);
}


void ReaderManager::disconnectAllReaders()
{
	const QMutexLocker mutexLocker(&mMutex);

	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::disconnectAllReaders, Qt::QueuedConnection);
}


void ReaderManager::updateRetryCounters()
{
	const QMutexLocker mutexLocker(&mMutex);

	QMetaObject::invokeMethod(mWorker.data(), &ReaderManagerWorker::updateRetryCounters, Qt::QueuedConnection);
}
