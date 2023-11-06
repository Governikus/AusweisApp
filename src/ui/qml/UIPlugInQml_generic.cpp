/**
 * Copyright (c) 2023 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugInQml.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

qreal UIPlugInQml::getSystemFontScaleFactor() const
{
	return 1.0;
}
