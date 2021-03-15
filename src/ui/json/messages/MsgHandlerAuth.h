/*!
 * \brief Message Auth of JSON API.
 *
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

#include "context/AuthContext.h"

namespace governikus
{

class MsgHandlerAuth
	: public MsgHandler
{
	private:
		QUrl createUrl(const QString& pUrl);
		void initAuth(const QUrl& pTcTokenUrl);
		void initMessages(const QJsonObject& pUi);
		void initHandleInterrupt(const QJsonValue& pValue);
		void setError(const QLatin1String pError);

	public:
		MsgHandlerAuth();
		explicit MsgHandlerAuth(const QJsonObject& pObj);
		explicit MsgHandlerAuth(const QSharedPointer<AuthContext>& pContext);
};


} // namespace governikus
