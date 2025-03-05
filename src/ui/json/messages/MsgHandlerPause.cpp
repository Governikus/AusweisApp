/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerPause.h"

#include <QJsonArray>

using namespace governikus;


MsgHandlerPause::MsgHandlerPause()
	: MsgHandler(MsgType::PAUSE)
{
	setCause(QLatin1String("BadCardPosition"));
}


MsgHandlerPause::MsgHandlerPause(MsgContext& pContext)
	: MsgHandlerPause()
{
	Q_ASSERT(pContext.getContext());

	pContext.getContext()->setStateApproved();
	setVoid();
}


void MsgHandlerPause::setCause(const QLatin1String pCause)
{
	setValue(QLatin1String("cause"), pCause);
}
