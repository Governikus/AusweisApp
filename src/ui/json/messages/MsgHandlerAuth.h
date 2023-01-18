/*!
 * \brief Message Auth of JSON API.
 *
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
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
		void initAuth(const QUrl& pTcTokenUrl);

	public:
		MsgHandlerAuth();
		explicit MsgHandlerAuth(const QJsonObject& pObj, MsgContext& pContext);
		explicit MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext);
};


} // namespace governikus
