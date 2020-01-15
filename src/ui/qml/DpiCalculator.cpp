/*
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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
#if defined(Q_OS_ANDROID)
		auto envDpi = qgetenv("QT_ANDROID_THEME_DISPLAY_DPI");
		bool ok;
		dpi = envDpi.toDouble(&ok);
		if (!ok)
		{
			qCCritical(qml) << "Cannot get screen dpi";
			dpi = -1.0;
		}
#elif defined(Q_OS_MACOS) || defined(Q_OS_IOS)
		// The physical DPI changes on Apple devices with screen scaling, on other platforms it behaves the other way around.
		dpi = screen->physicalDotsPerInch();
#else
		dpi = screen->logicalDotsPerInch();
#endif
		qCInfo(qml) << "Determined dpi:"
					<< screen->logicalDotsPerInch() << "(logical),"
					<< screen->physicalDotsPerInch() << "(physical)";
	}

	qCInfo(qml) << "Using dpi" << dpi;

#ifndef QT_NO_DEBUG
	const char* const overrideDpi = "OVERRIDE_DPI";
	if (!qEnvironmentVariableIsEmpty(overrideDpi))
	{
		dpi = qEnvironmentVariableIntValue(overrideDpi);
		qCDebug(qml) << "Override DPI:" << dpi;
	}
#endif

	return dpi;
}
