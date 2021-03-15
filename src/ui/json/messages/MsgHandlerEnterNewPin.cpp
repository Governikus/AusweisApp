/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterNewPin.h"

#include "context/ChangePinContext.h"

using namespace governikus;

MsgHandlerEnterNewPin::MsgHandlerEnterNewPin(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_NEW_PIN, pContext)
{
}


MsgHandlerEnterNewPin::MsgHandlerEnterNewPin(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerEnterNewPin(pContext)
{
	parseValue(pObj, pContext, [this, &pContext](const QString& pNumber)
			{
				auto ctx = pContext.getContext<ChangePinContext>();
				Q_ASSERT(ctx);
				ctx->setNewPin(pNumber);
				ctx->setStateApproved();
				setVoid();
			}, 6);
}
