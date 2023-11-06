/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerWorker.h"

#include "Initializer.h"
#include "Reader.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

#include <algorithm>

Q_DECLARE_LOGGING_CATEGORY(card)

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<CardConnectionWorker>>("QSharedPointer<CardConnectionWorker>");
		})


ReaderManagerWorker::ReaderManagerWorker()
	: QObject()
	, mPlugIns()
{
}


ReaderManagerWorker::~ReaderManagerWorker()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());
	qCDebug(card) << "Worker removed";
}


void ReaderManagerWorker::shutdown()
{
	qCDebug(card) << "Shutdown ReaderManagerWorker";
	for (auto& plugin : std::as_const(mPlugIns))
	{
		plugin->stopScan();

		qCDebug(card) << "Shutdown plugin:" << plugin->metaObject()->className();
		plugin->shutdown();

		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
	mPlugIns.clear();
}


void ReaderManagerWorker::onReaderRemoved(ReaderInfo pInfo)
{
	qCDebug(card) << "fireReaderRemoved:" << pInfo.getName();
	pInfo.invalidate();
	Q_EMIT fireReaderRemoved(pInfo);
}


void ReaderManagerWorker::onThreadStarted()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	qCDebug(card) << "Thread started";
	registerPlugIns();

	Q_EMIT fireInitialized();
}


void ReaderManagerWorker::registerPlugIns()
{
	qCDebug(card) << "Try to register plugins";
	const auto& plugins = QPluginLoader::staticPlugins();
	for (const auto& plugin : plugins)
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

				Q_EMIT firePluginAdded(pluginInstance->getInfo());
			}
		}
	}
}


bool ReaderManagerWorker::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("IID")).toString() == QLatin1String("governikus.ReaderManagerPlugIn");
}


void ReaderManagerWorker::registerPlugIn(ReaderManagerPlugIn* pPlugIn)
{
	Q_ASSERT(pPlugIn != nullptr);
	Q_ASSERT(!mPlugIns.contains(pPlugIn));

	mPlugIns << pPlugIn;

	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderAdded, this, &ReaderManagerWorker::fireReaderAdded);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderRemoved, this, &ReaderManagerWorker::onReaderRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireReaderPropertiesUpdated, this, &ReaderManagerWorker::fireReaderPropertiesUpdated);
	connect(pPlugIn, &ReaderManagerPlugIn::fireStatusChanged, this, &ReaderManagerWorker::fireStatusChanged);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardInserted, this, &ReaderManagerWorker::fireCardInserted);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardRemoved, this, &ReaderManagerWorker::fireCardRemoved);
	connect(pPlugIn, &ReaderManagerPlugIn::fireCardInfoChanged, this, &ReaderManagerWorker::fireCardInfoChanged);
}


void ReaderManagerWorker::callOnPlugIn(ReaderManagerPlugInType pType, const std::function<void(ReaderManagerPlugIn*)>& pFunc, const char* pLog)
{
	for (auto& plugin : std::as_const(mPlugIns))
	{
		if (plugin->getInfo().getPlugInType() == pType)
		{
			qCDebug(card).nospace() << pLog << ": " << plugin->metaObject()->className();
			pFunc(plugin);
		}
	}
}


void ReaderManagerWorker::reset(ReaderManagerPlugInType pType)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugIn(pType, [](ReaderManagerPlugIn* pPlugIn){
			pPlugIn->reset();
		}, "Reset plugin");
}


void ReaderManagerWorker::insert(const ReaderInfo& pReaderInfo, const QVariant& pData)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugIn(pReaderInfo.getPlugInType(), [pReaderInfo, pData](ReaderManagerPlugIn* pPlugIn){
			pPlugIn->insert(pReaderInfo.getName(), pData);
		}, "Insert");
}


void ReaderManagerWorker::shelve()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (const auto& plugIn : std::as_const(mPlugIns))
	{
		callOnPlugIn(plugIn->getInfo().getPlugInType(), [](ReaderManagerPlugIn* pPlugIn){
				pPlugIn->shelve();
			}, "Shelve");
	}
}


void ReaderManagerWorker::startScan(ReaderManagerPlugInType pType, bool pAutoConnect)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugIn(pType, [pAutoConnect](ReaderManagerPlugIn* pPlugIn){
			if (!pPlugIn->getInfo().isScanRunning())
			{
				pPlugIn->startScan(pAutoConnect);
			}
		}, "Start scan on plugin");
}


void ReaderManagerWorker::stopScan(ReaderManagerPlugInType pType, const QString& pError)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugIn(pType, [pError](ReaderManagerPlugIn* pPlugIn){
			if (pPlugIn->getInfo().isScanRunning())
			{
				pPlugIn->stopScan(pError);
			}
		}, "Stop scan on plugin");
}


QVector<ReaderInfo> ReaderManagerWorker::getReaderInfos() const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QVector<ReaderInfo> list;
	for (const auto& plugIn : std::as_const(mPlugIns))
	{
		const auto& readerList = plugIn->getReaders();
		for (const Reader* const reader : readerList)
		{
			list += reader->getReaderInfo();
		}
	}
	return list;
}


void ReaderManagerWorker::updateReaderInfo(const QString& pReaderName)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	Reader* reader = getReader(pReaderName);
	if (reader == nullptr)
	{
		qCWarning(card) << "Requested reader does not exist:" << pReaderName;
		return;
	}
	reader->updateCard();
}


Reader* ReaderManagerWorker::getReader(const QString& pReaderName) const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (auto& plugin : std::as_const(mPlugIns))
	{
		const auto& readerList = plugin->getReaders();
		for (Reader* reader : readerList)
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
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QSharedPointer<CardConnectionWorker> worker;
	if (auto reader = getReader(pReaderName))
	{
		worker = reader->createCardConnectionWorker();
	}
	Q_EMIT fireCardConnectionWorkerCreated(worker);
}
