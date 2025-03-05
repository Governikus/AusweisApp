/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerPersonalization.h"

#if __has_include("controller/PersonalizationController.h")
	#include "UiLoader.h"
	#include "UiPluginJson.h"
	#include "controller/PersonalizationController.h"

using namespace governikus;


MsgHandlerPersonalization::MsgHandlerPersonalization()
	: MsgHandlerWorkflows(MsgType::PERSONALIZATION)
{
}


MsgHandlerPersonalization::MsgHandlerPersonalization(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerPersonalization()
{
	const auto& jsonUrl = pObj[QLatin1String("appletServiceURL")];
	if (jsonUrl.isUndefined())
	{
		setError(QLatin1String("appletServiceURL cannot be undefined"));
	}
	else if (!jsonUrl.isString())
	{
		setError(QLatin1String("Invalid appletServiceURL"));
	}
	else
	{
		if (const auto& url = createUrl(jsonUrl.toString()); !url.isEmpty())
		{
			handleWorkflowProperties(pObj, pContext);
			initPersonalization(url);
			setVoid();
			return;
		}
		Q_ASSERT(isString(QLatin1String("error")));
	}
}


MsgHandlerPersonalization::MsgHandlerPersonalization(const QSharedPointer<PersonalizationContext>& pContext)
	: MsgHandlerPersonalization()
{
	Q_ASSERT(pContext);
	setValue(QLatin1String("result"), ECardApiResult(pContext->getStatus()).toJson());
}


QString MsgHandlerPersonalization::createUrl(const QString& pUrl)
{
	if (const QUrl parsedUrl(pUrl); parsedUrl.isValid() && !parsedUrl.host().isEmpty())
	{
		return pUrl;
	}

	setError(QLatin1String("Validation of appletServiceURL failed"));
	return QString();
}


void MsgHandlerPersonalization::initPersonalization(const QString& pAppletServiceURL)
{
	auto* ui = Env::getSingleton<UiLoader>()->getLoaded<UiPluginJson>();
	Q_ASSERT(ui);
	Q_EMIT ui->fireWorkflowRequested(PersonalizationController::createWorkflowRequest(pAppletServiceURL));
}


#endif
