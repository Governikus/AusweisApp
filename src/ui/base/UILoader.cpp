/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UILoader.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(gui)

using namespace governikus;


QStringList governikus::UILoader::cUserRequest;


UILoader::UILoader()
	: mLoadedPlugIns()
{
}


UILoader::~UILoader()
{
	if (!mLoadedPlugIns.isEmpty())
	{
		blockSignals(true);
		shutdown();
	}
}


QStringList UILoader::getInitialDefault()
{
	QStringList list;

	const auto& allPlugins = QPluginLoader::staticPlugins();
	for (auto& plugin : allPlugins)
	{
		const auto& metaData = plugin.metaData();
		if (isPlugIn(metaData) && isDefault(metaData))
		{
			list << getName(metaData);
		}
	}

	return list;
}


bool UILoader::isLoaded() const
{
	return !mLoadedPlugIns.isEmpty();
}


bool UILoader::load()
{
	bool any = false;
	const auto& list = getUserRequestOrDefault();
	for (const auto& entry : list)
	{
		any = load(entry) || any;
	}
	return any;
}


bool UILoader::load(const QString& pUi)
{
	Q_ASSERT(QObject::thread() == QThread::currentThread());

	const auto& requestedName = unify(pUi);
	if (mLoadedPlugIns.contains(requestedName))
	{
		return true;
	}

	qCDebug(gui) << "Try to load UI plugin:" << requestedName;

	const auto& allPlugins = QPluginLoader::staticPlugins();
	for (auto& plugin : allPlugins)
	{
		const auto& metaData = plugin.metaData();
		if (isPlugIn(metaData) && getName(metaData) == requestedName)
		{
			qCDebug(gui) << "Load plugin:" << metaData;
			auto instance = qobject_cast<UIPlugIn*>(plugin.instance());
			if (instance)
			{
				mLoadedPlugIns.insert(getName(instance->metaObject()), instance);
				Q_EMIT fireLoadedPlugin(instance);
				return true;
			}
			else
			{
				qCWarning(gui) << "Cannot cast to plugin instance:" << plugin.instance();
			}
		}
	}

	qCCritical(gui) << "Cannot find UI plugin:" << requestedName;
	return false;
}


QStringList UILoader::getUserRequestOrDefault()
{
	if (!cUserRequest.isEmpty())
	{
		return cUserRequest;
	}

	return getInitialDefault();
}


QString UILoader::getDefault()
{
	const auto& list = getInitialDefault();

	// Do not return empty QString here. Otherwise QCommandLineParser
	// won't accept any value for "--ui".
	return list.isEmpty() ? QStringLiteral(" ") : list.join(QLatin1Char(','));
}


void UILoader::setUserRequest(const QStringList& pRequest)
{
	if (pRequest.isEmpty())
	{
		cUserRequest.clear();
		return;
	}

	cUserRequest = pRequest;
}


void UILoader::shutdown()
{
	const auto& keys = mLoadedPlugIns.keys();
	qCDebug(gui) << "Shutdown UILoader:" << keys;

	for (const auto& key : keys)
	{
		UIPlugIn* const plugin = mLoadedPlugIns.value(key);

		connect(plugin, &QObject::destroyed, this, [this, key] {
				qCDebug(gui) << "Shutdown UI:" << key;
				mLoadedPlugIns.remove(key);
				if (mLoadedPlugIns.isEmpty())
				{
					Q_EMIT fireShutdownComplete();
				}
			}, Qt::QueuedConnection);

		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
}


bool UILoader::isDefault(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("MetaData")).toObject().value(QLatin1String("default")).toBool();
}


QString UILoader::getName(const QJsonObject& pJson)
{
	return unify(pJson.value(QLatin1String("className")).toString());
}


QString UILoader::getName(const QMetaObject* pMeta)
{
	return unify(QString::fromLatin1(pMeta->className()));
}


QString UILoader::unify(const QString& pName)
{
	return pName.toLower().remove(QLatin1String("governikus::")).remove(QLatin1String("uiplugin"));
}


bool UILoader::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("IID")).toString() == QLatin1String("governikus.UIPlugIn");
}
