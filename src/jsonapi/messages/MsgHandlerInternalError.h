/*!
 * \brief Message INTERNAL_ERROR of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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
		MsgHandlerInternalError(const QLatin1String& pError);
};


} /* namespace governikus */
