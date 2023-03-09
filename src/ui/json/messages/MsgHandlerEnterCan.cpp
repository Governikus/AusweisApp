/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterCan.h"

#include "context/WorkflowContext.h"

using namespace governikus;

MsgHandlerEnterCan::MsgHandlerEnterCan(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_CAN, pContext)
{
}


MsgHandlerEnterCan::MsgHandlerEnterCan(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerEnterCan(pContext)
{
	parseValue(pObj, pContext, [this, &pContext](const QString& pNumber)
		{
			auto ctx = pContext.getContext();
			ctx->setCan(pNumber);
			ctx->setStateApproved();
			setVoid();
		}, 6);
}
