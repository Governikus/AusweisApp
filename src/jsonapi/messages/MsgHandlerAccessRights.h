/*!
 * \brief Message MsgHandlerAccessRights of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"
#include "context/AuthContext.h"

namespace governikus
{

class MsgHandlerAccessRights
	: public MsgHandler
{
	private:
		void setError(const QLatin1String& pError);

		void handleSetRawData(const QJsonArray& pRaw, const QSharedPointer<AuthContext>& pContext);
		QJsonArray getAccessRights(const QSet<AccessRight>& pRights) const;
		void fillAccessRights(const QSharedPointer<const AuthContext>& pContext);

	public:
		MsgHandlerAccessRights(const MsgContext& pContext);
		MsgHandlerAccessRights(const QJsonObject& pObj, MsgContext& pContext);
};


} /* namespace governikus */
