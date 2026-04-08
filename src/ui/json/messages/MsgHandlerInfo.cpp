/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
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

	auto localIfd = QStringLiteral("UNKNOWN");
#ifdef Q_OS_ANDROID
	const auto& localIfdInfo = Env::getSingleton<ReaderManager>()->getPluginInfo(ReaderManagerPluginType::LOCAL_IFD);
	if (localIfdInfo.hasValue(ReaderManagerPluginInfo::Key::LOCAL_IFD_STATE))
	{
		localIfd = localIfdInfo.getValue(ReaderManagerPluginInfo::Key::LOCAL_IFD_STATE).toString();
	}
#endif

	setValue(QLatin1String("AusweisApp"), localIfd);
}
