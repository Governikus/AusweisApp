/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MsgHandlerInfo.h"

#ifdef Q_OS_ANDROID
	#include "ReaderManager.h"
#endif
#include "VersionInfo.h"


using namespace governikus;


MsgHandlerInfo::MsgHandlerInfo()
	: MsgHandler(MsgType::INFO)
{
	setValue(QLatin1String("VersionInfo"), VersionInfo::getInstance().toJsonObject());
}
