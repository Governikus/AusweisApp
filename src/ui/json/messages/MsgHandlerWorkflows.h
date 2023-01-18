/*!
 * \brief Generic class for Workflows.
 *
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
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
		void handleWorkflowProperties(const QJsonObject& pObj, MsgContext& pContext);

		void initMessages(const QJsonObject& pUi);
		void initDeveloperMode(const QJsonValue& pValue);
		void initHandleInterrupt(const QJsonValue& pValue, const MsgContext& pContext);
		void initProgressStatus(const QJsonValue& pValue, MsgContext& pContext);
		void setError(const QLatin1String pError);

		using MsgHandler::MsgHandler;
};


} // namespace governikus
