/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message Change PIN of JSON API.
 */

#pragma once

#include "MsgHandlerWorkflows.h"
#include "context/ChangePinContext.h"
#include "messages/MsgContext.h"

namespace governikus
{

class MsgHandlerChangePin
	: public MsgHandlerWorkflows
{
	public:
		MsgHandlerChangePin();
		explicit MsgHandlerChangePin(const QJsonObject& pObj, MsgContext& pContext);
		explicit MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext);
};


} // namespace governikus
