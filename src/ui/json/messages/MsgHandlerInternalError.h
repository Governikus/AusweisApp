/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerInternalError
	: public MsgHandler
{
	public:
		explicit MsgHandlerInternalError(const QString& pError = QString());
		explicit MsgHandlerInternalError(const QLatin1String pError);
};


} // namespace governikus
