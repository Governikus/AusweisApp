/*!
 * ActivationHandler.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "ActivationHandler.h"

#include "MetaTypeHelper.h"

#include <QAtomicPointer>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QUrlQuery>

REGISTER_META_TYPE(StatusFormat)
REGISTER_META_TYPE(UiModule)


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)


ActivationHandler::ActivationHandler()
{
}


ActivationHandler::~ActivationHandler()
{
}


QMap<QString, QString> ActivationHandler::getQueryParameter(const QUrl& pUrl)
{
	QMap<QString, QString> map;

	const auto queryItems = QUrlQuery(pUrl).queryItems();
	for (auto& item : queryItems)
	{
		map.insert(item.first.toLower(), item.second);
	}

	return map;
}


const QVector<ActivationHandler*>& ActivationHandler::getInstances()
{
	static QVector<ActivationHandler*> instances;

	if (instances.isEmpty())
	{
		qCDebug(activation) << "Try to register plugin";
		for (auto plugin : QPluginLoader::staticPlugins())
		{
			if (isPlugIn(plugin.metaData()))
			{
				qCDebug(activation) << "Load plugin:" << plugin.metaData();
				ActivationHandler* pluginInstance = qobject_cast<ActivationHandler*>(plugin.instance());
				if (pluginInstance == nullptr)
				{
					qCWarning(activation) << "Cannot cast to plugin instance:" << plugin.instance();
				}
				else
				{
					instances << pluginInstance;
				}
			}
		}
	}

	return instances;
}


bool ActivationHandler::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.ActivationHandler");
}
