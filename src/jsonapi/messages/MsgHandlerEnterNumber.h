/*!
 * \brief Helper handler for EnterCan, EnterPin and EnterPuk of JSON API.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"
#include "context/WorkflowContext.h"

#include <functional>

namespace governikus
{

class MsgHandlerEnterNumber
	: public MsgHandler
{
	private:
		void setError(const QString& pError);
		void setReader(const QSharedPointer<const WorkflowContext>& pContext);

	protected:
		MsgHandlerEnterNumber(MsgType pType, const MsgContext& pContext);

		void parseValue(const QJsonObject& pObj,
				const std::function<void(const QString& pNumber)>& pFunc,
				ushort pCount = 6);
};


} /* namespace governikus */
