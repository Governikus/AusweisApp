/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>
#ifdef Q_OS_IOS
	#include <QtDarwinHelpers>
#endif

#ifdef Q_OS_IOS
Q_FORWARD_DECLARE_OBJC_CLASS(UIWindowScene);
Q_FORWARD_DECLARE_OBJC_CLASS(UIWindow);
#endif

namespace governikus
{

class PlatformTools
{
	public:
		static void hideFromTaskbar();
		static void restoreToTaskbar();
		static void postNotification(const QString& pTitle, const QString& pMessage);
#ifdef Q_OS_IOS
		static UIWindowScene* getFirstWindowScene();
		static UIWindow* getFirstWindow();
#endif

};

} // namespace governikus
