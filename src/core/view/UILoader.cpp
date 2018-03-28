/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UILoader.h"

#include "SingletonHelper.h"
#include "UIPlugIn.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(gui)

using namespace governikus;

defineSingleton(UILoader)


#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#define DEFAULT_UI {UIPlugInName::UIPlugInQml}
#else
	#define DEFAULT_UI {UIPlugInName::UIPlugInWidgets}
#endif


UILoader::UILoader()
	: mLoadedPlugIns()
	, mDefault(DEFAULT_UI)
{
}


UILoader::~UILoader()
{
}


UILoader& UILoader::getInstance()
{
	return *Instance;
}


bool UILoader::load()
{
	bool any = false;
	for (auto entry : qAsConst(mDefault))
	{
		any = load(entry) || any;
	}
	return any;
}


bool UILoader::load(UIPlugInName pUi)
{
	Q_ASSERT(thread() == QThread::currentThread());

	if (mLoadedPlugIns.contains(pUi))
	{
		return true;
	}

	const auto& name = getEnumName(pUi);
	qCDebug(gui) << "Try to load UI plugin:" << name;

	const auto& allPlugins = QPluginLoader::staticPlugins();
	for (auto& plugin : allPlugins)
	{
		auto metaData = plugin.metaData();
		if (isPlugIn(metaData) && hasName(metaData, name))
		{
			qCDebug(gui) << "Load plugin:" << metaData;
			auto instance = qobject_cast<UIPlugIn*>(plugin.instance());
			if (instance)
			{
				mLoadedPlugIns.insert(pUi, instance);
				Q_EMIT fireLoadedPlugin(instance);
				return true;
			}
			else
			{
				qCWarning(gui) << "Cannot cast to plugin instance:" << plugin.instance();
			}
		}
	}

	qCCritical(gui) << "Cannot find UI plugin:" << name;
	return false;
}


UIPlugIn* UILoader::getLoaded(UIPlugInName pName) const
{
	return mLoadedPlugIns.value(pName);
}


bool UILoader::hasName(const QJsonObject& pJson, const QString& pName)
{
	return pJson.value(QStringLiteral("className")).toString() == pName;
}


bool UILoader::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.UIPlugIn");
}
