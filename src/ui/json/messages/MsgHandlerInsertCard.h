/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerInsertCard
	: public MsgHandler
{
	private:
		void setError(const QLatin1String pError);

	public:
		MsgHandlerInsertCard();
		explicit MsgHandlerInsertCard(MsgContext& pContext);
		explicit MsgHandlerInsertCard(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
