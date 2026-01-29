/**
 * Copyright (c) 2023-2026 Governikus GmbH & Co. KG, Germany
 */

#include "UiPluginQml.h"

#include "Env.h"
#include "SettingsModel.h"
#include "UiLoader.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(qml)

using namespace governikus;

QJniObject getAndroidResources_helper()
{
	QJniObject ctx = QNativeInterface::QAndroidApplication::context();
	if (!ctx.isValid())
	{
		qCWarning(qml) << "Android context is invalid" << ctx.toString();
		return QJniObject();
	}
	auto rsc = ctx.callObjectMethod(
			"getResources",
			"()Landroid/content/res/Resources;");
	if (!rsc.isValid())
	{
		qCWarning(qml) << "Android resources are invalid" << rsc.toString();
		return QJniObject();
	}
	return rsc;
}


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
	QJniObject rsc = getAndroidResources_helper();
	if (!rsc.isValid())
	{
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


int UiPluginQml::getSystemFontWeightAdjustment() const
{
	if (QJniObject::getStaticField<jint>("android/os/Build$VERSION", "SDK_INT") < 31) // Android 12
	{
		return 0;
	}
	QJniObject rsc = getAndroidResources_helper();
	if (!rsc.isValid())
	{
		return 0;
	}
	auto cfg = rsc.callObjectMethod("getConfiguration", "()Landroid/content/res/Configuration;");
	if (!cfg.isValid())
	{
		qCWarning(qml) << "Android configuration is invalid" << cfg.toString();
		return 0;
	}
	return cfg.getField<jint>("fontWeightAdjustment");
}


}


void UiPluginQml::onUserDarkModeChanged() const
{
	if (QJniObject activity = QNativeInterface::QAndroidApplication::context(); activity.isValid())
	{
		activity.callMethod<void>("setAppearanceLightStatusBars", "(Z)V", !isDarkModeEnabled());
	}
}


bool UiPluginQml::getA11yButtonShapeActive() const
{
	return false;
}


bool UiPluginQml::getA11yOnOffSwitchLabelActive() const
{
	return false;
}
