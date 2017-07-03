/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
	parseValue(pObj["can"], [&](const QString& pNumber)
	{
		auto ctx = pContext.getWorkflowContext();
		ctx->setCan(pNumber);
		ctx->setStateApproved();
		setVoid();
	});
}
