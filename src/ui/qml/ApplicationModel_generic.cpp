/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QLoggingCategory>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(qml)

void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
}


bool ApplicationModel::isScreenReaderRunning() const
{
	qCWarning(qml) << "NOT IMPLEMENTED";
	return false;
}
