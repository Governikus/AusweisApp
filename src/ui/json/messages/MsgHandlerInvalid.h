/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

#include <QJsonParseError>

namespace governikus
{

class MsgHandlerInvalid
	: public MsgHandler
{
	public:
		explicit MsgHandlerInvalid(const QString& pError = QString());
		explicit MsgHandlerInvalid(const QLatin1String pError);
		explicit MsgHandlerInvalid(const QJsonParseError& pError);
};


} // namespace governikus
