/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DpiCalculator.h"

#include <QGuiApplication>
#include <QLoggingCategory>
#include <QScreen>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)


qreal DpiCalculator::getDpi()
{
	qreal dpi = 0.0;
	auto app = static_cast<QGuiApplication*>(QCoreApplication::instance());
	auto screen = app->primaryScreen();
	if (screen)
	{
		dpi = screen->logicalDotsPerInch();
	}

	qCInfo(qml) << "Determined dpi" << dpi;

#ifndef QT_NO_DEBUG
	const char* overrideDpi = "OVERRIDE_DPI";
	if (!qEnvironmentVariableIsEmpty(overrideDpi))
	{
		dpi = qEnvironmentVariableIntValue(overrideDpi);
		qCDebug(qml) << "Override DPI:" << dpi;
	}
#endif

	return dpi;
}
