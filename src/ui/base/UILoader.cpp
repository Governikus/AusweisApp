/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "UILoader.h"

#include "AppSettings.h"
#include "SingletonHelper.h"

#include <QLoggingCategory>
#include <QPluginLoader>
#include <QThread>


Q_DECLARE_LOGGING_CATEGORY(gui)


using namespace governikus;


defineSingleton(UILoader)


namespace
{
QString getPrefixUi()
{
	return QStringLiteral("UIPlugIn");
}


} // namespace


UILoader::UILoader()
	: mLoadedPlugIns()
	, mDefault()
{
	QStringList list({Env::getSingleton<AppSettings>()->getGeneralSettings().getSelectedUi()});

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
	list << getName(UIPlugInName::UIPlugInWebSocket);
#endif

	setDefault(list);
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
	Q_ASSERT(QObject::thread() == QThread::currentThread());

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


const QStringList UILoader::getDefault() const
{
	QStringList list;
	for (auto entry : qAsConst(mDefault))
	{
		list << getName(entry);
	}
	return list;
}


void UILoader::setDefault(const QStringList& pDefault)
{
	QVector<UIPlugInName> selectedPlugins;
	const auto& availablePlugins = Enum<UIPlugInName>::getList();

	for (const auto& parsedUiOption : pDefault)
	{
		for (auto availablePluginEntry : availablePlugins)
		{
			if (parsedUiOption.compare(QString(getEnumName(availablePluginEntry)).remove(getPrefixUi()), Qt::CaseInsensitive) == 0)
			{
				selectedPlugins << availablePluginEntry;
			}
		}
	}

	if (!selectedPlugins.isEmpty())
	{
		mDefault = selectedPlugins;
	}
}


UIPlugIn* UILoader::getLoaded(UIPlugInName pName) const
{
	return mLoadedPlugIns.value(pName);
}


void UILoader::shutdown()
{
	const QList<UIPlugInName> keys = mLoadedPlugIns.keys();
	for (UIPlugInName key : keys)
	{
		UIPlugIn* const plugin = mLoadedPlugIns.value(key);

		connect(plugin, &QObject::destroyed, this,
				[ = ](){
					mLoadedPlugIns.remove(key);
					if (mLoadedPlugIns.isEmpty())
					{
						Q_EMIT fireShutdownComplete();
					}
				},
				Qt::QueuedConnection);

		// Plugins and therefore their members are not auto destructed due to a bug in Qt.
		// https://bugreports.qt.io/browse/QTBUG-17458
		plugin->deleteLater();
	}
}


bool UILoader::hasName(const QJsonObject& pJson, const QString& pName)
{
	return pJson.value(QStringLiteral("className")).toString() == pName;
}


QString UILoader::getName(UIPlugInName pPlugin) const
{
	return QString(getEnumName(pPlugin)).remove(getPrefixUi());
}


bool UILoader::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.UIPlugIn");
}
