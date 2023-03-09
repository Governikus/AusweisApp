/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerEnterNewPin.h"

#include "context/ChangePinContext.h"
#if __has_include("context/PersonalizationContext.h")
	#include "context/PersonalizationContext.h"
#endif

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
#if __has_include("context/PersonalizationContext.h")
			else if (auto persoCtx = pContext.getContext<PersonalizationContext>(); persoCtx)
			{
				persoCtx->setNewPin(pNumber);
				persoCtx->setStateApproved();
			}
#endif
			else
			{
				Q_ASSERT(false);
			}

			setVoid();
		}, 6);
}
