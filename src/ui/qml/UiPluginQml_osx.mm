/**
 * Copyright (c) 2019-2026 Governikus GmbH & Co. KG, Germany
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


int UiPluginQml::getSystemFontWeightAdjustment() const
{
	return 0;
}


bool UiPluginQml::getA11yButtonShapeActive() const
{
	return false;
}


bool UiPluginQml::getA11yOnOffSwitchLabelActive() const
{
	return false;
}
