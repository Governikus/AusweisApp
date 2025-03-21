/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "MsgContext.h"
#include "MsgHandler.h"
#include "ReaderInfo.h"

namespace governikus
{

class MsgHandlerReader
	: public MsgHandler
{
	private:
		void setError(const QLatin1String pError);
		void setReaderInfo(const ReaderInfo& pInfo, const MsgContext& pContext);

	public:
		static QJsonObject createReaderInfo(const ReaderInfo& pInfo, const MsgContext& pContext);

		explicit MsgHandlerReader(const QJsonObject& pObj, const MsgContext& pContext);
		explicit MsgHandlerReader(const ReaderInfo& pInfo, const MsgContext& pContext);
};


} // namespace governikus
