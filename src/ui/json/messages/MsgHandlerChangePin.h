/*!
 * \brief Message Change PIN of JSON API.
 *
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgHandler.h"

#include "context/ChangePinContext.h"

namespace governikus
{

class MsgHandlerChangePin
	: public MsgHandler
{
	private:
		void initMessages(const QJsonObject& pUi);
		void initHandleInterrupt(const QJsonValue& pValue);

	public:
		MsgHandlerChangePin();
		explicit MsgHandlerChangePin(const QJsonObject& pObj);
		explicit MsgHandlerChangePin(const QSharedPointer<const ChangePinContext>& pContext);
};


} // namespace governikus
