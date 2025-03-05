/**
 * Copyright (c) 2024-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QLoggingCategory>

#include <windows.h>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)


bool ApplicationModel::isScreenReaderRunning() const
{
	BOOL screenReaderRunning = false;
	const BOOL success = SystemParametersInfo(SPI_GETSCREENREADER, 0, &screenReaderRunning, 0);
	return success && screenReaderRunning;
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
}
