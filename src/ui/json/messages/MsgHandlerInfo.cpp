/*!
 * \copyright Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInfo.h"

#include "Env.h"
#include "ReaderManager.h"
#include "ReaderManagerPlugInInfo.h"
#include "VersionInfo.h"

using namespace governikus;

MsgHandlerInfo::MsgHandlerInfo()
	: MsgHandler(MsgType::INFO)
{
	mJsonObject[QLatin1String("VersionInfo")] = VersionInfo::getInstance().toJsonObject();
}
