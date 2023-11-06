/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include "Env.h"
#include "UILoader.h"

#include <QLoggingCategory>

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

bool UIPlugInQml::isHighContrastEnabled() const
{
	return NSWorkspace.sharedWorkspace.accessibilityDisplayShouldIncreaseContrast;
}


qreal UIPlugInQml::getSystemFontScaleFactor() const
{
	return 1.0;
}
