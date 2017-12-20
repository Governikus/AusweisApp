/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterPuk.h"

#include "context/WorkflowContext.h"

using namespace governikus;

MsgHandlerEnterPuk::MsgHandlerEnterPuk(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_PUK, pContext)
{
}


MsgHandlerEnterPuk::MsgHandlerEnterPuk(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerEnterPuk(pContext)
{
	parseValue(pObj, [&](const QString& pNumber)
			{
				auto ctx = pContext.getWorkflowContext();
				ctx->setPuk(pNumber);
				ctx->setStateApproved();
				setVoid();
			}, 10);
}
