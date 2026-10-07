/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#include "AppUpdateDataModel.h"

#include "AppSettings.h"
#include "Env.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(update)


extern "C" {

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifyUpdateFound(JNIEnv* pEnv, jobject pObj, jint pVersionCode, jint pStalenessDays, jint pPriority)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)
	QMetaObject::invokeMethod(QCoreApplication::instance(), [pVersionCode = static_cast<int>(pVersionCode), pStalenessDays = static_cast<int>(pStalenessDays), pPriority = static_cast<int>(pPriority)] {
				auto* appUpdateDataModel = Env::getSingleton<AppUpdateDataModel>();
				if (!appUpdateDataModel)
				{
					qCWarning(update) << "Failed to retrieve AppUpdateDataModel to handle found update";
					return;
				}

				if (pPriority >= 4)
				{
					qCInfo(update) << "Found high-priority update, starting immediate update flow";
					appUpdateDataModel->startUpdateFlow(true);
					return;
				}

				auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
				if (pVersionCode > settings.getUpdateVersionCode())
				{
					qCInfo(update) << "Found update with versionCode" << pVersionCode;
					settings.setUpdateVersionCode(pVersionCode);
					settings.setUpdateReminderShown(false);
					appUpdateDataModel->startUpdateFlow(false);
				}
				else if (pVersionCode == settings.getUpdateVersionCode())
				{
					if (pStalenessDays >= 30 && !settings.isUpdateReminderShown())
					{
						qCInfo(update) << "Update with versionCode" << pVersionCode << "available since" << pStalenessDays << "reminding user to update";
						settings.setUpdateReminderShown(true);
						appUpdateDataModel->startUpdateFlow(false);
					}
				}
			}, Qt::QueuedConnection);
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifyUpdateReadyForInstallation(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)
	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				auto* appUpdateDataModel = Env::getSingleton<AppUpdateDataModel>();
				if (appUpdateDataModel)
				{
					Q_EMIT appUpdateDataModel->fireUpdateAvailable();
				}
			}, Qt::QueuedConnection);
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifyImmediateUpdateCanceled(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)
	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				auto* appUpdateDataModel = Env::getSingleton<AppUpdateDataModel>();
				if (appUpdateDataModel)
				{
					Q_EMIT appUpdateDataModel->fireUpdateCanceled();
				}
			}, Qt::QueuedConnection);
}


}
void AppUpdateDataModel::applyUpdate() const
{
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	context.callMethod<void>("completeUpdate");
}


void AppUpdateDataModel::startUpdateFlow(bool pImmediate) const
{
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	context.callMethod<void>("startUpdateFlow", "(Z)V", pImmediate);
}
