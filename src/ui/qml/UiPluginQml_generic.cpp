/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

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
