/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManager.h"

#include "CardConnection.h"
#include "SingletonHelper.h"

#include <QLoggingCategory>

using namespace governikus;

defineSingleton(ReaderManager)

Q_DECLARE_LOGGING_CATEGORY(card)


ReaderManager::ReaderManager()
	: QObject()
	, mThread()
	, mWorker()
	, mRemoteClient()
{
	mThread.setObjectName(QStringLiteral("ReaderManagerThread"));
}


ReaderManager::~ReaderManager()
{
	if (mThread.isRunning())
	{
		qCWarning(card) << "ReaderManager is not stopped correctly...";
		shutdown();
	}
}


ReaderManager& ReaderManager::getInstance()
{
	return *Instance;
}


void ReaderManager::init(const QSharedPointer<RemoteClient>& pRemoteClient)
{
	if (mThread.isRunning())
	{
		qCWarning(card) << "ReaderManager already initialized";
		return;
	}

	if (mWorker.isNull())
	{
		mRemoteClient = pRemoteClient;
		mWorker = new ReaderManagerWorker(pRemoteClient);
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
	if (mThread.isRunning() && !mThread.isInterruptionRequested())
	{
		qCDebug(card) << "Shutdown ReaderManager...";
		mThread.requestInterruption(); // do not try to stop AGAIN from dtor
		mThread.quit();
		mThread.wait(2500);
		qCDebug(card) << "Stopping..." << mThread.isRunning();
	}
}


void ReaderManager::startScan(ReaderManagerPlugInType pType)
{
	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot start scan if ReaderManager-Thread is not active";
		return;
	}

	QMetaObject::invokeMethod(mWorker.data(), "startScan", Qt::QueuedConnection, Q_ARG(ReaderManagerPlugInType, pType));
}


void ReaderManager::startScanAll()
{
	for (const auto& plugInType : Enum<ReaderManagerPlugInType>::getList())
	{
		startScan(plugInType);
	}
}


void ReaderManager::stopScan(ReaderManagerPlugInType pType)
{
	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot stop scan if ReaderManager-Thread is not active";
		return;
	}

	QMetaObject::invokeMethod(mWorker.data(), "stopScan", Qt::QueuedConnection, Q_ARG(ReaderManagerPlugInType, pType));
}


void ReaderManager::stopScanAll()
{
	for (const auto& plugInType : Enum<ReaderManagerPlugInType>::getList())
	{
		stopScan(plugInType);
	}
}


QVector<ReaderManagerPlugInInfo> ReaderManager::getPlugInInfos() const
{
	QVector<ReaderManagerPlugInInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), "getPlugInInfos", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVector<ReaderManagerPlugInInfo>, list));
	return list;
}


QVector<ReaderInfo> ReaderManager::getReaderInfos(ReaderManagerPlugInType pType) const
{
	return getReaderInfos(QVector<ReaderManagerPlugInType>() << pType);
}


QVector<ReaderInfo> ReaderManager::getReaderInfos(const ReaderFilter& pFilter) const
{
	QVector<ReaderInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), "getReaderInfos", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVector<ReaderInfo>, list), Q_ARG(ReaderFilter, pFilter));
	return list;
}


ReaderInfo ReaderManager::getReaderInfo(const QString& pReaderName) const
{
	ReaderInfo info(pReaderName);
	QMetaObject::invokeMethod(mWorker.data(), "getReaderInfo", Qt::BlockingQueuedConnection, Q_RETURN_ARG(ReaderInfo, info), Q_ARG(QString, pReaderName));
	return info;
}


void ReaderManager::connectReader(const QString& pReaderName)
{
	QMetaObject::invokeMethod(mWorker.data(), "connectReader", Qt::QueuedConnection, Q_ARG(QString, pReaderName));
}


void ReaderManager::disconnectReader(const QString& pReaderName)
{
	QMetaObject::invokeMethod(mWorker.data(), "disconnectReader", Qt::QueuedConnection, Q_ARG(QString, pReaderName));
}


void ReaderManager::disconnectAllReaders()
{
	QMetaObject::invokeMethod(mWorker.data(), "disconnectAllReaders", Qt::QueuedConnection);
}


QSharedPointer<RemoteClient> ReaderManager::getRemoteClient()
{
	return mRemoteClient;
}
