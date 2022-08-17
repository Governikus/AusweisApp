/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationController.h"

#include <QLoggingCategory>
#include <QPluginLoader>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(activation)


ActivationController::ActivationController()
	: mInstances()
{
	qCDebug(activation) << "Register activation plugins...";
	const auto& plugins = QPluginLoader::staticPlugins();
	for (const auto& plugin : plugins)
	{
		if (isPlugIn(plugin.metaData()))
		{
			qCDebug(activation) << "Load plugin:" << plugin.metaData();
			if (ActivationHandler* pluginInstance = qobject_cast<ActivationHandler*>(plugin.instance()); pluginInstance == nullptr)
			{
				qCWarning(activation) << "Cannot cast to plugin instance:" << plugin.instance();
			}
			else
			{
				QObject::connect(pluginInstance, &QObject::destroyed, pluginInstance, [plugin] {
						qCDebug(activation) << "Destroy ActivationHandler:" << plugin.metaData();
					});
				mInstances << pluginInstance;
			}
		}
	}
}


ActivationController::~ActivationController()
{
	shutdown();
}


void ActivationController::shutdown()
{
	for (const auto& handler : qAsConst(mInstances))
	{
		handler->stop();
		handler->deleteLater();
	}
	mInstances.clear();
}


const QVector<ActivationHandler*>& ActivationController::getHandler() const
{
	return mInstances;
}


bool ActivationController::isPlugIn(const QJsonObject& pJson)
{
	return pJson.value(QLatin1String("IID")).toString() == QLatin1String("governikus.ActivationHandler");
}
