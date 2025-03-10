/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"
#include "messages/MsgContext.h"

namespace governikus
{

class MsgHandlerWorkflows
	: public MsgHandler
{
	protected:
		void handleWorkflowProperties(const QJsonObject& pObj, MsgContext& pContext) const;

		void initMessages(const QJsonObject& pUi) const;
		void initDeveloperMode(const QJsonValue& pValue) const;
		void initHandleInterrupt(const QJsonValue& pValue, const MsgContext& pContext) const;
		void initProgressStatus(const QJsonValue& pValue, MsgContext& pContext) const;
		void setError(const QLatin1String pError);

		using MsgHandler::MsgHandler;
};


} // namespace governikus
