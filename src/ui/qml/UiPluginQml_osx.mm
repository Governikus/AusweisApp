/**
 * Copyright (c) 2019-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "Env.h"
#include "UiLoader.h"

#include <QLoggingCategory>

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

bool UiPluginQml::isHighContrastEnabled() const
{
	return NSWorkspace.sharedWorkspace.accessibilityDisplayShouldIncreaseContrast;
}


qreal UiPluginQml::getSystemFontScaleFactor() const
{
	return 1.0;
}
