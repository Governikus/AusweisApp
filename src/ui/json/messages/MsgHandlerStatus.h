/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message Status of JSON API.
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

#include <QJsonValue>

class test_MsgHandlerStatus;

namespace governikus
{

class MsgHandlerStatus
	: public MsgHandler
{
	friend class ::test_MsgHandlerStatus;

	private:
		QJsonValue getWorkflow(const MsgContext& pContext) const;
		QJsonValue getProgress(const MsgContext& pContext) const;
		QJsonValue getState(const MsgContext& pContext) const;

	public:
		explicit MsgHandlerStatus(const MsgContext& pContext);
};

} /* namespace governikus */
