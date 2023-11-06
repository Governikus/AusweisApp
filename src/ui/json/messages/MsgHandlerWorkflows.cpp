/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerWorkflows.h"

#include "VolatileSettings.h"

using namespace governikus;


void MsgHandlerWorkflows::handleWorkflowProperties(const QJsonObject& pObj, MsgContext& pContext)
{
	initMessages(pObj[QLatin1String("messages")].toObject());
	initDeveloperMode(pObj[QLatin1String("developerMode")]);
	initHandleInterrupt(pObj[QLatin1String("handleInterrupt")], pContext);
	initProgressStatus(pObj[QLatin1String("status")], pContext);
}


void MsgHandlerWorkflows::initMessages(const QJsonObject& pUi) const
{
	if (!pUi.isEmpty())
	{
		const VolatileSettings::Messages messages(pUi[QLatin1String("sessionStarted")].toString(),
				pUi[QLatin1String("sessionFailed")].toString(),
				pUi[QLatin1String("sessionSucceeded")].toString(),
				pUi[QLatin1String("sessionInProgress")].toString());

		Env::getSingleton<VolatileSettings>()->setMessages(messages);
	}
}


void MsgHandlerWorkflows::initHandleInterrupt(const QJsonValue& pValue, const MsgContext& pContext) const
{
	if (pContext.getApiLevel() < MsgLevel::v2)
	{
		Env::getSingleton<VolatileSettings>()->setHandleInterrupt(pValue.isBool() ? pValue.toBool() : true);
	}
}


void MsgHandlerWorkflows::initDeveloperMode(const QJsonValue& pValue) const
{
	if (pValue.isBool())
	{
		Env::getSingleton<VolatileSettings>()->setDeveloperMode(pValue.toBool());
		qDebug() << "Using Developer Mode on SDK:" << Env::getSingleton<VolatileSettings>()->isDeveloperMode();
	}
}


void MsgHandlerWorkflows::initProgressStatus(const QJsonValue& pValue, MsgContext& pContext) const
{
	if (pValue.isBool())
	{
		pContext.setProgressStatus(pValue.toBool());
	}
}


void MsgHandlerWorkflows::setError(const QLatin1String pError)
{
	setValue("error", pError);
}
