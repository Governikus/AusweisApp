/*!
 * \brief Message MsgHandlerAccessRights of JSON API.
 *
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "MsgContext.h"
#include "MsgHandler.h"

#include <functional>

namespace governikus
{

class MsgHandlerAccessRights
	: public MsgHandler
{
	private:
		void setError(const QLatin1String pError);

		void handleSetChatData(const QJsonArray& pChat, const QSharedPointer<AuthContext>& pContext);
		QJsonArray getAccessRights(const QSet<AccessRight>& pRights) const;
		void fillAccessRights(const QSharedPointer<const AuthContext>& pContext);
		QJsonObject getAuxiliaryData(const QSharedPointer<const AuthContext>& pContext);

	public:
		explicit MsgHandlerAccessRights(const MsgContext& pContext);
		explicit MsgHandlerAccessRights(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
