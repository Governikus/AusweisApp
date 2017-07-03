/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerEnterPin.h"

#include "context/WorkflowContext.h"

using namespace governikus;

MsgHandlerEnterPin::MsgHandlerEnterPin(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_PIN, pContext)
{
}


MsgHandlerEnterPin::MsgHandlerEnterPin(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerEnterPin(pContext)
{
	parseValue(pObj["pin"], [&](const QString& pNumber)
	{
		auto ctx = pContext.getWorkflowContext();
		ctx->setPin(pNumber);
		ctx->setStateApproved();
		setVoid();
	});
}
