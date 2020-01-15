/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationHandler.h"

#include "ActivationContext.h"
#include "Initializer.h"

#include <QAtomicPointer>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QUrlQuery>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)

static Initializer::Entry X([] {
			qRegisterMetaType<UiModule>("UiModule");
			qRegisterMetaType<QSharedPointer<ActivationContext> >("QSharedPointer<ActivationContext>");
		});


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
		const auto& plugins = QPluginLoader::staticPlugins();
		for (const auto& plugin : plugins)
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


void ActivationHandler::onApplicationActivated()
{
}


bool ActivationHandler::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QStringLiteral("IID")).toString() == QLatin1String("governikus.ActivationHandler");
}
