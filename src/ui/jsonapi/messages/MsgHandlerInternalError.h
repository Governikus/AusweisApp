/*!
 * \brief Message INTERNAL_ERROR of JSON API.
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerInternalError
	: public MsgHandler
{
	public:
		MsgHandlerInternalError(const QString& pError = QString());
		MsgHandlerInternalError(const QLatin1String pError);
};


} // namespace governikus
