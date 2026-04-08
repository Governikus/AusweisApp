/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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
				if (auto pinCtx = pContext.getContext<ChangePinContext>(); pinCtx)
				{
					pinCtx->setNewPin(pNumber);
					pinCtx->setStateApproved();
				}
				else
				{
					Q_ASSERT(false);
				}

				setVoid();
			}, 6);
}
