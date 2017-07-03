/*!
 * ReaderManager.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
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


void ReaderManager::init()
{
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

		connect(mWorker.data(), &ReaderManagerWorker::fireStatusChanged, this, &ReaderManager::fireStatusChanged);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardInserted, this, &ReaderManager::fireCardInserted);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRemoved, this, &ReaderManager::fireCardRemoved);
		connect(mWorker.data(), &ReaderManagerWorker::fireCardRetryCounterChanged, this, &ReaderManager::fireCardRetryCounterChanged);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderPropertiesUpdated, this, &ReaderManager::fireReaderPropertiesUpdated);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderAdded, this, &ReaderManager::fireReaderAdded);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderDeviceError, this, &ReaderManager::fireReaderDeviceError);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderConnected, this, &ReaderManager::fireReaderConnected);
		connect(mWorker.data(), &ReaderManagerWorker::fireReaderRemoved, this, &ReaderManager::fireReaderRemoved);

		connect(this, &ReaderManager::fireReaderAdded, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireReaderRemoved, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireCardInserted, this, &ReaderManager::fireReaderEvent);
		connect(this, &ReaderManager::fireCardRemoved, this, &ReaderManager::fireReaderEvent);
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
		qCDebug(card) << "Stopping..." << mThread.wait(2500);
	}
}


void ReaderManager::startScan()
{
	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot start scan if ReaderManager-Thread is not active";
		return;
	}

	QMetaObject::invokeMethod(mWorker.data(), "startScan", Qt::QueuedConnection);
}


void ReaderManager::stopScan()
{
	if (!mThread.isRunning())
	{
		qCWarning(card) << "Cannot stop scan if ReaderManager-Thread is not active";
		return;
	}

	QMetaObject::invokeMethod(mWorker.data(), "stopScan", Qt::QueuedConnection);
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


QVector<ReaderInfo> ReaderManager::getReaderInfos(const QVector<ReaderManagerPlugInType>& pTypes) const
{
	QVector<ReaderInfo> list;
	QMetaObject::invokeMethod(mWorker.data(), "getReaderInfos", Qt::BlockingQueuedConnection, Q_RETURN_ARG(QVector<ReaderInfo>, list), Q_ARG(const QVector<ReaderManagerPlugInType> &, pTypes));
	return list;
}


ReaderInfo ReaderManager::getReaderInfo(const QString& pReaderName) const
{
	ReaderInfo info;
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
