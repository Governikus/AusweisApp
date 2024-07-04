/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

qreal UiPluginQml::getSystemFontScaleFactor() const
{
	return 1.0;
}
