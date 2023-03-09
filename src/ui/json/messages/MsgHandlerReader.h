/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Message Reader of JSON API.
 */

#pragma once

#include "MsgHandler.h"
#include "ReaderInfo.h"

namespace governikus
{

class MsgHandlerReader
	: public MsgHandler
{
	private:
		static void setReaderInfo(QJsonObject& pObj, const ReaderInfo& pInfo);

		void setError(const QLatin1String pError);
		void setReaderInfo(const ReaderInfo& pInfo);

	public:
		static QJsonObject createReaderInfo(const ReaderInfo& pInfo);

		explicit MsgHandlerReader(const QJsonObject& pObj);
		explicit MsgHandlerReader(const ReaderInfo& pInfo);
};


} // namespace governikus
