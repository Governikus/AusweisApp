/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
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
	, mPlugins()
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
	for (auto& plugin : std::as_const(mPlugins))
	{
		plugin->stopScan();

		qCDebug(card) << "Shutdown plugin:" << plugin->metaObject()->className();
		plugin->shutdown();

		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
	mPlugins.clear();
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
	registerPlugins();

	Q_EMIT fireInitialized();
}


void ReaderManagerWorker::registerPlugins()
{
	qCDebug(card) << "Try to register plugins";
	const auto& plugins = QPluginLoader::staticPlugins();
	for (const auto& plugin : plugins)
	{
		if (isPlugin(plugin.metaData()))
		{
			qCDebug(card) << "Register and initialize plugin:" << plugin.metaData();
			ReaderManagerPlugin* pluginInstance = qobject_cast<ReaderManagerPlugin*>(plugin.instance());
			if (pluginInstance == nullptr)
			{
				qCWarning(card) << "Cannot cast to plugin instance:" << plugin.instance();
			}
			else
			{
				registerPlugin(pluginInstance);
				pluginInstance->init();

				Q_EMIT firePluginAdded(pluginInstance->getInfo());
			}
		}
	}
}


bool ReaderManagerWorker::isPlugin(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("IID")).toString() == QLatin1String("governikus.ReaderManagerPlugin");
}


void ReaderManagerWorker::registerPlugin(ReaderManagerPlugin* pPlugin)
{
	Q_ASSERT(pPlugin != nullptr);
	Q_ASSERT(!mPlugins.contains(pPlugin));

	mPlugins << pPlugin;

	connect(pPlugin, &ReaderManagerPlugin::fireReaderAdded, this, &ReaderManagerWorker::fireReaderAdded);
	connect(pPlugin, &ReaderManagerPlugin::fireReaderRemoved, this, &ReaderManagerWorker::onReaderRemoved);
	connect(pPlugin, &ReaderManagerPlugin::fireReaderPropertiesUpdated, this, &ReaderManagerWorker::fireReaderPropertiesUpdated);
	connect(pPlugin, &ReaderManagerPlugin::fireStatusChanged, this, &ReaderManagerWorker::fireStatusChanged);
	connect(pPlugin, &ReaderManagerPlugin::fireCardInserted, this, &ReaderManagerWorker::fireCardInserted);
	connect(pPlugin, &ReaderManagerPlugin::fireCardRemoved, this, &ReaderManagerWorker::fireCardRemoved);
	connect(pPlugin, &ReaderManagerPlugin::fireCardInfoChanged, this, &ReaderManagerWorker::fireCardInfoChanged);
}


void ReaderManagerWorker::callOnPlugin(ReaderManagerPluginType pType, const std::function<void(ReaderManagerPlugin*)>& pFunc, const char* pLog)
{
	for (auto& plugin : std::as_const(mPlugins))
	{
		if (plugin->getInfo().getPluginType() == pType)
		{
			qCDebug(card).nospace() << pLog << ": " << plugin->metaObject()->className();
			pFunc(plugin);
		}
	}
}


void ReaderManagerWorker::reset(ReaderManagerPluginType pType)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugin(pType, [](ReaderManagerPlugin* pPlugin){
			pPlugin->reset();
		}, "Reset plugin");
}


void ReaderManagerWorker::insert(const ReaderInfo& pReaderInfo, const QVariant& pData)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugin(pReaderInfo.getPluginType(), [pReaderInfo, pData](ReaderManagerPlugin* pPlugin){
			pPlugin->insert(pReaderInfo.getName(), pData);
		}, "Insert");
}


void ReaderManagerWorker::shelve()
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	for (const auto& plugin : std::as_const(mPlugins))
	{
		callOnPlugin(plugin->getInfo().getPluginType(), [](ReaderManagerPlugin* pPlugin){
				pPlugin->shelve();
			}, "Shelve");
	}
}


void ReaderManagerWorker::startScan(ReaderManagerPluginType pType, bool pAutoConnect)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugin(pType, [pAutoConnect](ReaderManagerPlugin* pPlugin){
			if (!pPlugin->getInfo().isScanRunning())
			{
				pPlugin->startScan(pAutoConnect);
			}
		}, "Start scan on plugin");
}


void ReaderManagerWorker::stopScan(ReaderManagerPluginType pType, const QString& pError)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	callOnPlugin(pType, [pError](ReaderManagerPlugin* pPlugin){
			if (pPlugin->getInfo().isScanRunning())
			{
				pPlugin->stopScan(pError);
				pPlugin->setInitialScanState(ReaderManagerPluginInfo::InitialScan::UNKNOWN);
			}
		}, "Stop scan on plugin");
}


QList<ReaderInfo> ReaderManagerWorker::getReaderInfos() const
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QList<ReaderInfo> list;
	for (const auto& plugin : std::as_const(mPlugins))
	{
		const auto& readerList = plugin->getReaders();
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

	for (auto& plugin : std::as_const(mPlugins))
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


void ReaderManagerWorker::createCardConnectionWorker(const QString& pReaderName, const std::function<QSharedPointer<CardConnectionWorker>(const QSharedPointer<CardConnectionWorker>&)>& pInitWorker)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	QSharedPointer<CardConnectionWorker> worker;
	if (auto reader = getReader(pReaderName))
	{
		worker = reader->createCardConnectionWorker();
		if (pInitWorker)
		{
			worker = pInitWorker(worker);
		}
	}
	Q_EMIT fireCardConnectionWorkerCreated(worker);
}
