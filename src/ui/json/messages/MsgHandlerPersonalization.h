/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#if __has_include("context/PersonalizationContext.h")

	#include "MsgHandlerWorkflows.h"
	#include "context/PersonalizationContext.h"
	#include "messages/MsgContext.h"

namespace governikus
{

class MsgHandlerPersonalization
	: public MsgHandlerWorkflows
{
	private:
		QString createUrl(const QString& pUrl);
		void initPersonalization(const QString& pAppletServiceURL);

	public:
		MsgHandlerPersonalization();
		explicit MsgHandlerPersonalization(const QJsonObject& pObj, MsgContext& pContext);
		explicit MsgHandlerPersonalization(const QSharedPointer<PersonalizationContext>& pContext);
};


} // namespace governikus

#endif
