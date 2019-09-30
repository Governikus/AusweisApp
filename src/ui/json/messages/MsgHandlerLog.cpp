/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerLog.h"

#include "Env.h"
#include "LogHandler.h"

using namespace governikus;

MsgHandlerLog::MsgHandlerLog()
	: MsgHandler(MsgType::LOG)
{
	const auto& data = Env::getSingleton<LogHandler>()->getBacklog();
	mJsonObject[QLatin1String("data")] = QString::fromUtf8(data);
}
