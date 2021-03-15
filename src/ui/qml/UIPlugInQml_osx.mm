/*!
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include <AppKit/AppKit.h>

using namespace governikus;

bool UIPlugInQml::isHighContrastEnabled() const
{
	return NSWorkspace.sharedWorkspace.accessibilityDisplayShouldIncreaseContrast;
}
