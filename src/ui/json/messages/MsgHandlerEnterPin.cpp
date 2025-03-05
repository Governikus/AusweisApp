/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterPin.h"

#include "context/ChangePinContext.h"

using namespace governikus;

MsgHandlerEnterPin::MsgHandlerEnterPin(const MsgContext& pContext)
	: MsgHandlerEnterNumber(MsgType::ENTER_PIN, pContext)
{
}


MsgHandlerEnterPin::MsgHandlerEnterPin(const QJsonObject& pObj, MsgContext& pContext)
	: MsgHandlerEnterPin(pContext)
{
	const ushort minPinLength = pContext.getContext<const ChangePinContext>() ? 5 : 6;
	parseValue(pObj, pContext, [this, &pContext](const QString& pNumber)
			{
				auto ctx = pContext.getContext();
				ctx->setPin(pNumber);
				ctx->setStateApproved();
				setVoid();
			}, {minPinLength, 6});
}
