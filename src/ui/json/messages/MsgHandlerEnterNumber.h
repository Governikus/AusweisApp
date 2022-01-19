/*!
 * \brief Helper handler for EnterCan, EnterPin and EnterPuk of JSON API.
 *
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "MsgContext.h"
#include "MsgHandler.h"

#include <functional>
#include <tuple>

namespace governikus
{

class MsgHandlerEnterNumber
	: public MsgHandler
{
	private:
		void setError(const QString& pError);
		void setReader(const QSharedPointer<const WorkflowContext>& pContext);

	protected:
		explicit MsgHandlerEnterNumber(MsgType pType, const MsgContext& pContext);

		void parseValue(const QJsonObject& pObj,
			const MsgContext& pContext,
			const std::function<void(const QString& pNumber)>& pFunc,
			ushort pCount)
		{
			parseValue(pObj, pContext, pFunc, {pCount, pCount});
		}


		void parseValue(const QJsonObject& pObj,
				const MsgContext& pContext,
				const std::function<void(const QString& pNumber)>& pFunc,
				std::pair<ushort, ushort> pCount);
};


} // namespace governikus
