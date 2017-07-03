/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "Reader.h"
#include "ReaderManagerWorker.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

ReaderManagerWorker::ReaderManagerWorker()
	: QObject()
	, mPlugIns()
	, mScanStarted(false)
{
	qRegisterMetaType<QSharedPointer<CardConnectionWorker> >("QSharedPointer<CardConnectionWorker>");
}


ReaderManagerWorker::~ReaderManagerWorker()
{
	Q_ASSERT(thread() == QThread::currentThread());

	for (auto plugin : qAsConst(mPlugIns))
	{
		qCDebug(card) << "Shutdown plugin:" << plugin->metaObject()->className();
		plugin->shutdown();
	}
}


void ReaderManagerWorker::onThreadStarted()
{
	Q_ASSERT(thread() == QThread::currentThread());

	qCDebug(card) << "Thread started";
	registerPlugIns();
}


void ReaderManagerWorker::registerPlugIns()
{
	qCDebug(card) << "Try to register plugins";
	for (auto plugin : QPluginLoader::staticPlugins())
	{
		if (isPlugIn(plugin.metaData()))
		{
			qCDebug(card) << "Register and initialize plugin:" << plugin.metaData();
			ReaderManagerPlugIn* pluginInstance = qobject_cast<ReaderManagerPlugIn*>(plugin.instance());
			if (pluginInstance == nullptr)
			{
				qCWarning(card) << "Cannot cast to plugin instance:" << plugin.instance();
			}
			else
			{
				registerPlugIn(pluginInstance);
				pluginInstance->init();
			}
		}
	}
}


bool ReaderManagerWorker::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.ReaderManagerPlugIn");
}


void ReaderManagerWorker::registerPlugIn(ReaderManagerPlugIn* pPlugIn)
{
	Q_ASSERT(pPlugIn != nullptr);
	Q_ASSERT(!mPlugIns.contains(pPlugIn));

	mPlugIns.push_back(pPlugIn);

	connect(pPlugIn, &ReaderManagerPlugIn::fireStatusChanged, this, &ReaderManagerWorker::fireStatusChanged);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardInserted, this, &ReaderManagerWorker::fireCardInserted);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardRemoved, this, &ReaderManagerWorker::fireCardRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardRetryCounterChanged, this, &ReaderManagerWorker::fireCardRetryCounterChanged);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderAdded, this, &ReaderManagerWorker::fireReaderAdded);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderConnected, this, &ReaderManagerWorker::fireReaderConnected);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderRemoved, this, &ReaderManagerWorker::fireReaderRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderPropertiesUpdated, this, &ReaderManagerWorker::fireReaderPropertiesUpdated);
}


void ReaderManagerWorker::startScan()
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (mScanStarted)
	{
		qCWarning(card) << "Scan is already started, skip starting";
		return;
	}

	mScanStarted = true;
	for (auto plugin : qAsConst(mPlugIns))
	{
		qCDebug(card) << "Start scan on plugin:" << plugin->metaObject()->className();
		plugin->startScan();
	}
}


void ReaderManagerWorker::stopScan()
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (!mScanStarted)
	{
		qCWarning(card) << "Scan is not started, skip stopping";
		return;
	}

	mScanStarted = false;
	for (auto plugin : qAsConst(mPlugIns))
	{
		qCDebug(card) << "Stop scan on plugin:" << plugin->metaObject()->className();
		plugin->stopScan();
	}
}


QVector<ReaderManagerPlugInInfo> ReaderManagerWorker::getPlugInInfos() const
{
	Q_ASSERT(thread() == QThread::currentThread());

	QVector<ReaderManagerPlugInInfo> infos;
	infos.reserve(mPlugIns.size());
	for (const ReaderManagerPlugIn* plugIn : mPlugIns)
	{
		infos += plugIn->getInfo();
	}

	return infos;
}


QVector<ReaderInfo> ReaderManagerWorker::getReaderInfos(const QVector<ReaderManagerPlugInType>& pTypes) const
{
	Q_ASSERT(thread() == QThread::currentThread());

	QVector<ReaderInfo> list;
	for (const auto plugIn : qAsConst(mPlugIns))
	{
		if (pTypes.contains(plugIn->getInfo().getPlugInType()))
		{
			for (const Reader* reader : plugIn->getReader())
			{
				list += reader->getReaderInfo();
			}
		}
	}
	return list;
}


ReaderInfo ReaderManagerWorker::getReaderInfo(const QString& pReaderName) const
{
	Q_ASSERT(thread() == QThread::currentThread());

	const Reader* reader = getReader(pReaderName);
	return reader ? reader->getReaderInfo() : ReaderInfo();
}


Reader* ReaderManagerWorker::getReader(const QString& pReaderName) const
{
	Q_ASSERT(thread() == QThread::currentThread());

	for (auto plugin : mPlugIns)
	{
		for (Reader* reader : plugin->getReader())
		{
			if (reader->getName() == pReaderName)
			{
				return reader;
			}
		}
	}

	qCWarning(card) << "Requested reader does not exist:" << pReaderName;
	return nullptr;
}


void ReaderManagerWorker::createCardConnectionWorker(const QString& pReaderName)
{
	Q_ASSERT(thread() == QThread::currentThread());

	QSharedPointer<CardConnectionWorker> worker;
	if (auto reader = getReader(pReaderName))
	{
		worker = reader->createCardConnectionWorker();
	}
	Q_EMIT fireCardConnectionWorkerCreated(worker);
}


void ReaderManagerWorker::connectReader(const QString& pReaderName)
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (ConnectableReader* reader = dynamic_cast<ConnectableReader*>(getReader(pReaderName)))
	{
		reader->connectReader();
	}
}


void ReaderManagerWorker::disconnectReader(const QString& pReaderName)
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (ConnectableReader* reader = dynamic_cast<ConnectableReader*>(getReader(pReaderName)))
	{
		reader->disconnectReader();
	}
}


void ReaderManagerWorker::disconnectAllReaders()
{
	Q_ASSERT(thread() == QThread::currentThread());

	for (auto& info : getReaderInfos(EnumReaderManagerPlugInType::getList()))
	{
		disconnectReader(info.getName());
	}
}
