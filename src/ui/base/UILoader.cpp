/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "UILoader.h"

#include "BuildHelper.h"

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

	list.sort();
	return list;
}


bool UILoader::initialize() const
{
	return std::all_of(mLoadedPlugIns.begin(), mLoadedPlugIns.end(), [](UIPlugIn* pUi){
			return pUi->initialize();
		});
}


bool UILoader::hasActiveUI() const
{
	return std::any_of(mLoadedPlugIns.begin(), mLoadedPlugIns.end(), [](const UIPlugIn* pUi){
			return !pUi->property("passive").toBool();
		});
}


bool UILoader::requiresReaderManager() const
{
	return std::any_of(mLoadedPlugIns.begin(), mLoadedPlugIns.end(), [](const UIPlugIn* pUi){
			const auto& property = pUi->property("readerManager");
			return property.isNull() || property.toBool();
		});
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
			if (!BuildHelper::isUserInteractive() && isUserInteractive(metaData))
			{
				qCDebug(gui) << "Cannot load plugin in non-interactive mode:" << metaData;
				continue;
			}

			qCDebug(gui) << "Load plugin:" << metaData;
			auto instance = qobject_cast<UIPlugIn*>(plugin.instance());
			if (instance)
			{
				preparePlugIn(instance, metaData);
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


void UILoader::preparePlugIn(UIPlugIn* pUi, const QJsonObject& pMetaData)
{
	setMetaDataProperties(pUi, pMetaData);
	const auto& key = getName(pUi->metaObject());

	mLoadedPlugIns.insert(key, pUi);
	connect(pUi, &QObject::destroyed, this, [this, key] {
			qCDebug(gui) << "Shutdown UI:" << key;
			mLoadedPlugIns.remove(key);

			if (mLoadedPlugIns.isEmpty())
			{
				Q_EMIT fireRemovedAllPlugins();
			}
		});

	Q_EMIT fireLoadedPlugin(pUi);
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
	qCDebug(gui) << "Shutdown UILoader:" << mLoadedPlugIns.keys();

	for (const auto& plugin : std::as_const(mLoadedPlugIns))
	{
		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
}


void UILoader::setMetaDataProperties(UIPlugIn* pUi, const QJsonObject& pJson)
{
	const auto& fileContent = getMetaDataFileContent(pJson);
	const auto& iterEnd = fileContent.constEnd();
	for (auto iter = fileContent.constBegin(); iter != iterEnd; ++iter)
	{
		const bool qProperty = pUi->setProperty(qPrintable(iter.key()), iter.value().toVariant());
		Q_ASSERT(!qProperty);
	}
}


QJsonObject UILoader::getMetaDataFileContent(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("MetaData")).toObject();
}


bool UILoader::isDefault(const QJsonObject& pJson)
{
	return getMetaDataFileContent(pJson).value(QLatin1String("default")).toBool();
}


bool UILoader::isUserInteractive(const QJsonObject& pJson)
{
	return getMetaDataFileContent(pJson).value(QLatin1String("userInteractive")).toBool();
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
