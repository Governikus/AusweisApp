/*!
 * \brief Message Invalid of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
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
		MsgHandlerInvalid(const QString& pError = QString());
		MsgHandlerInvalid(const QLatin1String pError);
		MsgHandlerInvalid(const QJsonParseError& pError);
};


} // namespace governikus
