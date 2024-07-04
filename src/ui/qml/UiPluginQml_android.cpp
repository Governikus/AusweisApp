/**
 * Copyright (c) 2023-2024 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "Env.h"
#include "UiLoader.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;


extern "C"
{
JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifyConfigurationChanged(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)
	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
			if (auto* uiPlugin = Env::getSingleton<UiLoader>()->getLoaded<UiPluginQml>())
			{
				Q_EMIT uiPlugin->fireAppConfigChanged();
			}
		}, Qt::QueuedConnection);
}


qreal UiPluginQml::getSystemFontScaleFactor() const
{
	QJniObject ctx = QNativeInterface::QAndroidApplication::context();
	if (!ctx.isValid())
	{
		qCWarning(qml) << "Android context is invalid" << ctx.toString();
		return 1.0;
	}
	auto rsc = ctx.callObjectMethod(
			"getResources",
			"()Landroid/content/res/Resources;");
	if (!rsc.isValid())
	{
		qCWarning(qml) << "Android resources are invalid" << rsc.toString();
		return 1.0;
	}
	auto cfg = rsc.callObjectMethod("getConfiguration", "()Landroid/content/res/Configuration;");
	if (!cfg.isValid())
	{
		qCWarning(qml) << "Android configuration is invalid" << cfg.toString();
		return 1.0;
	}
	auto displayMetrics = rsc.callObjectMethod("getDisplayMetrics", "()Landroid/util/DisplayMetrics;");
	if (!displayMetrics.isValid())
	{
		qCWarning(qml) << "Android DisplayMetrics object is invalid" << displayMetrics.toString();
		return 1.0;
	}
	const auto density = displayMetrics.getField<jfloat>("density");
	const auto scaledDensity = displayMetrics.getField<jfloat>("scaledDensity");
	return scaledDensity / density;
}


}
