/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiLoader.h"

#include "BuildHelper.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(gui)

using namespace governikus;


QStringList governikus::UiLoader::cUserRequest;


UiLoader::UiLoader()
	: mLoadedPlugins()
{
}


UiLoader::~UiLoader()
{
	if (!mLoadedPlugins.isEmpty())
	{
		blockSignals(true);
		shutdown();
	}
}


QStringList UiLoader::getInitialDefault()
{
	QStringList list;

	const auto& allPlugins = QPluginLoader::staticPlugins();
	for (auto& plugin : allPlugins)
	{
		const auto& metaData = plugin.metaData();
		if (isPlugin(metaData) && isDefault(metaData))
		{
			list << getName(metaData);
		}
	}

	list.sort();
	return list;
}


bool UiLoader::initialize() const
{
	return std::all_of(mLoadedPlugins.begin(), mLoadedPlugins.end(), [](UiPlugin* pUi){
			return pUi->initialize();
		});
}


bool UiLoader::hasActiveUI() const
{
	return std::any_of(mLoadedPlugins.begin(), mLoadedPlugins.end(), [](const UiPlugin* pUi){
			return !pUi->property("passive").toBool();
		});
}


bool UiLoader::requiresReaderManager() const
{
	return std::any_of(mLoadedPlugins.begin(), mLoadedPlugins.end(), [](const UiPlugin* pUi){
			const auto& property = pUi->property("readerManager");
			return property.isNull() || property.toBool();
		});
}


bool UiLoader::isLoaded() const
{
	return !mLoadedPlugins.isEmpty();
}


bool UiLoader::load()
{
	bool any = false;
	const auto& list = getUserRequestOrDefault();
	for (const auto& entry : list)
	{
		any = load(entry) || any;
	}
	return any;
}


bool UiLoader::load(const QString& pUi)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	const auto& requestedName = unify(pUi);
	if (mLoadedPlugins.contains(requestedName))
	{
		return true;
	}

	qCDebug(gui) << "Try to load UI plugin:" << requestedName;

	const auto& allPlugins = QPluginLoader::staticPlugins();
	for (auto& plugin : allPlugins)
	{
		const auto& metaData = plugin.metaData();
		if (isPlugin(metaData) && getName(metaData) == requestedName)
		{
			if (!BuildHelper::isUserInteractive() && isUserInteractive(metaData))
			{
				qCDebug(gui) << "Cannot load plugin in non-interactive mode:" << metaData;
				continue;
			}

			setEnvironment(metaData);
			qCDebug(gui) << "Load plugin:" << metaData;
			auto instance = qobject_cast<UiPlugin*>(plugin.instance());
			if (!instance)
			{
				qCWarning(gui) << "Cannot cast to plugin instance:" << plugin.instance();
				continue;
			}

			preparePlugin(instance, metaData);
			return true;
		}
	}

	qCCritical(gui) << "Cannot find UI plugin:" << requestedName;
	return false;
}


void UiLoader::preparePlugin(UiPlugin* pUi, const QJsonObject& pMetaData)
{
	setMetaDataProperties(pUi, pMetaData);
	const auto& key = getName(pUi->metaObject());

	mLoadedPlugins.insert(key, pUi);
	connect(pUi, &QObject::destroyed, this, [this, key] {
			qCDebug(gui) << "Shutdown UI:" << key;
			mLoadedPlugins.remove(key);

			if (mLoadedPlugins.isEmpty())
			{
				Q_EMIT fireRemovedAllPlugins();
			}
		});

	Q_EMIT fireLoadedPlugin(pUi);
}


QStringList UiLoader::getUserRequestOrDefault()
{
	if (!cUserRequest.isEmpty())
	{
		return cUserRequest;
	}

	return getInitialDefault();
}


QString UiLoader::getDefault()
{
	const auto& list = getInitialDefault();

	// Do not return empty QString here. Otherwise QCommandLineParser
	// won't accept any value for "--ui".
	return list.isEmpty() ? QStringLiteral(" ") : list.join(QLatin1Char(','));
}


void UiLoader::setUserRequest(const QStringList& pRequest)
{
	if (pRequest.isEmpty())
	{
		cUserRequest.clear();
		return;
	}

	cUserRequest = pRequest;
}


void UiLoader::shutdown()
{
	qCDebug(gui) << "Shutdown UiLoader:" << mLoadedPlugins.keys();

	for (const auto& plugin : std::as_const(mLoadedPlugins))
	{
		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
}


void UiLoader::setMetaDataProperties(UiPlugin* pUi, const QJsonObject& pJson)
{
	const auto& fileContent = getMetaDataFileContent(pJson);
	const auto& iterEnd = fileContent.constEnd();
	for (auto iter = fileContent.constBegin(); iter != iterEnd; ++iter)
	{
		const bool qProperty = pUi->setProperty(qPrintable(iter.key()), iter.value().toVariant());
		Q_ASSERT(!qProperty);
	}
}


QJsonObject UiLoader::getMetaDataFileContent(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("MetaData")).toObject();
}


bool UiLoader::isDefault(const QJsonObject& pJson)
{
	return getMetaDataFileContent(pJson).value(QLatin1String("default")).toBool();
}


bool UiLoader::isUserInteractive(const QJsonObject& pJson)
{
	return getMetaDataFileContent(pJson).value(QLatin1String("userInteractive")).toBool();
}


QString UiLoader::getName(const QJsonObject& pJson)
{
	return unify(pJson.value(QLatin1String("className")).toString());
}


QString UiLoader::getName(const QMetaObject* pMeta)
{
	return unify(QString::fromLatin1(pMeta->className()));
}


QString UiLoader::unify(const QString& pName)
{
	return pName.toLower().remove(QLatin1String("governikus::")).remove(QLatin1String("uiplugin"));
}


bool UiLoader::isPlugin(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("IID")).toString() == QLatin1String("governikus.UiPlugin");
}


void UiLoader::setEnvironment(const QJsonObject& pJson)
{
	const auto& envMap = getMetaDataFileContent(pJson).value(QLatin1String("env")).toObject().toVariantMap();
	for (const auto [key, value] : envMap.asKeyValueRange())
	{
		const auto stringValue = value.toString().toLatin1();
		qCDebug(gui) << "Set environment:" << key << "=" << stringValue;
		qputenv(qPrintable(key), stringValue);
	}
}
