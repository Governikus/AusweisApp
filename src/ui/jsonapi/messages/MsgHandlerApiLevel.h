/*!
 * \brief Message API_LEVEL of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"

namespace governikus
{

class MsgHandlerApiLevel
	: public MsgHandler
{
	private:
		void setError(const QLatin1String pError);
		void setCurrentLevel(MsgLevel pLevel);
		void setAvailableLevel();

	public:
		MsgHandlerApiLevel(const MsgContext& pContext);
		MsgHandlerApiLevel(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
