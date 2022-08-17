/*!
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ActivationHandler.h"

#include "ActivationContext.h"
#include "AppSettings.h"
#include "Env.h"
#include "Initializer.h"

using namespace governikus;


INIT_FUNCTION([] {
			qRegisterMetaType<QSharedPointer<ActivationContext>>("QSharedPointer<ActivationContext>");
		})


void ActivationHandler::handleQueryParams(const QUrlQuery& pUrl)
{
	const auto queryUseTestUri = QLatin1String("useTestUri");
	if (pUrl.hasQueryItem(queryUseTestUri))
	{
		const auto value = pUrl.queryItemValue(queryUseTestUri);
		const bool useTestUri = QVariant(value).toBool();
		Env::getSingleton<AppSettings>()->getGeneralSettings().setUseSelfauthenticationTestUri(useTestUri);
	}

	const auto queryEnableSimulator = QLatin1String("enableSimulator");
	if (pUrl.hasQueryItem(queryEnableSimulator))
	{
		const auto value = pUrl.queryItemValue(queryEnableSimulator);
		const bool enableSimulator = QVariant(value).toBool();
		Env::getSingleton<AppSettings>()->getGeneralSettings().setSimulatorEnabled(enableSimulator);
	}
}


ActivationHandler::ActivationRequest ActivationHandler::getRequest(const QUrlQuery& pUrl)
{
	const auto queryItems = pUrl.queryItems();
	for (const auto& item : queryItems)
	{
		ActivationType type = Enum<ActivationType>::fromString(item.first.toUpper(), ActivationType::UNKNOWN);
		if (type != ActivationType::UNKNOWN)
		{
			return {type, item.second};
		}
	}

	return {ActivationType::UNKNOWN, QString()};
}


void ActivationHandler::onApplicationActivated()
{
}
