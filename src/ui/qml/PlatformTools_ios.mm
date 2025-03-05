/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PlatformTools.h"

#include <QLoggingCategory>

#import <UIKit/UIKit.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


void PlatformTools::hideFromTaskbar()
{
}


void PlatformTools::restoreToTaskbar()
{
}


void PlatformTools::postNotification(const QString& pTitle, const QString& pMessage)
{
	Q_UNUSED(pTitle)
	Q_UNUSED(pMessage)
}


UIWindowScene* PlatformTools::getFirstWindowScene()
{
	auto connectedScenes = [UIApplication sharedApplication].connectedScenes;
	if (connectedScenes.count == 0)
	{
		qCWarning(qml) << "No connected scenes found.";
		return nullptr;
	}

	UIScene* scene = connectedScenes.allObjects.firstObject;
	UIWindowScene* windowScene = static_cast<UIWindowScene*>(scene);
	if (!windowScene)
	{
		qCWarning(qml) << "Could not get window from scene.";
		return nullptr;
	}
	return windowScene;
}


UIWindow* PlatformTools::getFirstWindow()
{
	UIWindowScene* windowScene = getFirstWindowScene();
	if (!windowScene)
	{
		return nullptr;
	}

	auto windows = windowScene.windows;
	if (windows.count == 0)
	{
		qCWarning(qml) << "No windows connected to scene.";
		return nullptr;
	}

	return windows.firstObject;
}
