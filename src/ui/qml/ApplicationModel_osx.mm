/**
 * Copyright (c) 2024 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <AppKit/NSAccessibility.h>

using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)


bool ApplicationModel::isScreenReaderRunning() const
{
	return NSWorkspace.sharedWorkspace.voiceOverEnabled;
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
}
