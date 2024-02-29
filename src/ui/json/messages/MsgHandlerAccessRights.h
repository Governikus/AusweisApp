/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message MsgHandlerAccessRights of JSON API.
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"
#include "context/AuthContext.h"

#include <functional>

namespace governikus
{

class MsgHandlerAccessRights
	: public MsgHandler
{
	private:
		void setError(const QLatin1String pError);

		void handleSetChatData(const QJsonArray& pChat, const QSharedPointer<AuthContext>& pContext);
		[[nodiscard]] QJsonArray getAccessRights(const QSet<AccessRight>& pRights) const;
		[[nodiscard]] QJsonArray getAcceptedEidTypes(const QSharedPointer<const AuthContext>& pContext) const;
		void fillAccessRights(const QSharedPointer<const AuthContext>& pContext);
		[[nodiscard]] QJsonObject getAuxiliaryData(const QSharedPointer<const AuthContext>& pContext) const;

	public:
		explicit MsgHandlerAccessRights(const MsgContext& pContext);
		explicit MsgHandlerAccessRights(const QJsonObject& pObj, MsgContext& pContext);
};


} // namespace governikus
