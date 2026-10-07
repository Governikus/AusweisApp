/**
 * Copyright (c) 2016-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "MsgHandlerWorkflows.h"
#include "context/AuthContext.h"
#include "messages/MsgContext.h"

namespace governikus
{

class MsgHandlerAuth
	: public MsgHandlerWorkflows
{
	private:
		QUrl createUrl(const QString& pUrl);
		AuthContext::HeaderMap createMap(const QJsonValue& pCustomHeader);
		void initAuth(const QUrl& pTcTokenUrl, const AuthContext::HeaderMap& pCustomHeader) const;

	public:
		MsgHandlerAuth();
		explicit MsgHandlerAuth(const QJsonObject& pObj, MsgContext& pContext);
		explicit MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext);
};


} // namespace governikus
