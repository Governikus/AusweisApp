/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
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
	mJsonObject[QLatin1String("VersionInfo")] = VersionInfo::getInstance().toJsonObject();

	auto localIfd = QStringLiteral("UNKNOWN");
#ifdef Q_OS_ANDROID
	const auto& localIfdInfo = Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::LOCAL_IFD);
	if (localIfdInfo.hasValue(ReaderManagerPlugInInfo::Key::LOCAL_IFD_STATE))
	{
		localIfd = localIfdInfo.getValue(ReaderManagerPlugInInfo::Key::LOCAL_IFD_STATE).toString();
	}
#endif

	mJsonObject[QLatin1String("AusweisApp")] = localIfd;
}
