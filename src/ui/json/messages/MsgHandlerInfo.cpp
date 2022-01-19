/*!
 * \copyright Copyright (c) 2016-2022 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInfo.h"

#include "VersionInfo.h"

using namespace governikus;

MsgHandlerInfo::MsgHandlerInfo()
	: MsgHandler(MsgType::INFO)
{
	mJsonObject[QLatin1String("VersionInfo")] = VersionInfo::getInstance().toJsonObject();
}
