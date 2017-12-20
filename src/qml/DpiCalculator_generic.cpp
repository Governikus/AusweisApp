/*
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
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

#if defined(Q_OS_ANDROID)
	auto qtDpi = qgetenv("QT_ANDROID_THEME_DISPLAY_DPI");
	bool ok;
	dpi = qtDpi.toFloat(&ok);
	if (!ok)
	{
		qCCritical(qml) << "Cannot get screen dpi";
		return -1.0;
	}

#elif defined(Q_OS_IOS)
	qCCritical(qml) << "Determination of dpi on iOS not supported";
	return -1.0;

#else
	auto app = static_cast<QGuiApplication*>(QCoreApplication::instance());
	auto screen = app->primaryScreen();
	dpi = screen->logicalDotsPerInch();
#endif
	qCInfo(qml) << "Determined dpi" << dpi;

#ifndef Q_NO_DEBUG
	const char* overrideDpi = "OVERRIDE_DPI";
	if (!qEnvironmentVariableIsEmpty(overrideDpi))
	{
		dpi = qEnvironmentVariableIntValue(overrideDpi);
		qCDebug(qml) << "Override DPI:" << dpi;
	}
#endif

	return dpi;

}
