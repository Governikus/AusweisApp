/*
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "DpiCalculator.h"

#include <QLoggingCategory>
#include <QtGlobal>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(qml)


qreal DpiCalculator::getDpi()
{
	auto qtDpi = qgetenv("QT_ANDROID_THEME_DISPLAY_DPI");
	bool ok;
	qreal dpi = qtDpi.toDouble(&ok);
	if (!ok)
	{
		qCCritical(qml) << "Cannot get screen dpi";
		return -1.0;
	}

	qCInfo(qml) << "Determined dpi" << dpi;

	return dpi;
}
