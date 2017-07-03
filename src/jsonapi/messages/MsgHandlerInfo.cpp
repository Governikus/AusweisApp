/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#include "MsgHandlerInfo.h"

#include "VersionInfo.h"

using namespace governikus;

MsgHandlerInfo::MsgHandlerInfo()
	: MsgHandler(MsgType::INFO)
{
	mJsonObject["VersionInfo"] = VersionInfo::getInstance().toJsonObject();
}
