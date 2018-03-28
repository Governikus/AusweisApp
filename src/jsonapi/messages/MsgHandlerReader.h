/*!
 * \brief Message Reader of JSON API.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
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
		void setReaderInfo(const QString& pName);

	public:
		static QJsonObject createReaderInfo(const ReaderInfo& pInfo);

		MsgHandlerReader(const QJsonObject& pObj);
		MsgHandlerReader(const QString& pName);
};


} /* namespace governikus */
