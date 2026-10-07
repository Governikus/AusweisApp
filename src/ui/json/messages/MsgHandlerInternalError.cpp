/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#include "MsgHandlerInternalError.h"

using namespace governikus;

MsgHandlerInternalError::MsgHandlerInternalError(const QString& pError)
	: MsgHandler(MsgType::INTERNAL_ERROR, "error", pError)
{
}


MsgHandlerInternalError::MsgHandlerInternalError(const QLatin1String pError)
	: MsgHandler(MsgType::INTERNAL_ERROR, "error", pError)
{
}
