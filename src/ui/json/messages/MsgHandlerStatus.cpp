/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerStatus.h"

using namespace governikus;

MsgHandlerStatus::MsgHandlerStatus(const MsgContext& pContext)
	: MsgHandler(MsgType::STATUS)
{
	setValue(QLatin1String("workflow"), getWorkflow(pContext));
	setValue(QLatin1String("progress"), getProgress(pContext));
	setValue(QLatin1String("state"), getState(pContext));
}


QJsonValue MsgHandlerStatus::getWorkflow(const MsgContext& pContext) const
{
	if (pContext.isActiveWorkflow())
	{
		switch (pContext.getContext()->getAction())
		{
			case Action::AUTH:
				return QLatin1String("AUTH");

			case Action::PIN:
				return QLatin1String("CHANGE_PIN");

			case Action::PERSONALIZATION:
				return QLatin1String("PERSONALIZATION");

			// SDK do NOT support those workflows
			case Action::SELF:
			case Action::REMOTE_SERVICE:
				return QLatin1String("UNKNOWN");
		}
	}

	return QJsonValue::Null;
}


QJsonValue MsgHandlerStatus::getProgress(const MsgContext& pContext) const
{
	if (pContext.isActiveWorkflow())
	{
		const auto& ctx = pContext.getContext();
		return ctx.isNull() ? 0 : ctx->getProgressValue();
	}

	return QJsonValue::Null;
}


QJsonValue MsgHandlerStatus::getState(const MsgContext& pContext) const
{
	if (pContext.isActiveWorkflow())
	{
		const auto& msg = pContext.getLastStateMsg();
		if (msg != MsgType::VOID)
		{
			return getEnumName(MsgType(msg));
		}
	}

	return QJsonValue::Null;
}
