/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QOperatingSystemVersion>
#include <QtCore/private/qandroidextras_p.h>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(feedback)
Q_DECLARE_LOGGING_CATEGORY(qml)

extern "C" {

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_MainActivity_notifyScreenReaderRunningChanged(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pEnv)
	Q_UNUSED(pObj)
	ApplicationModel::notifyScreenReaderChangedThreadSafe();
}


}


static void showSystemSettings(const QString& pAction)
{
	QJniEnvironment env;

	const QJniObject& jAction = QJniObject::fromString(pAction);
	QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	const jint flag = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK");
	intent.callObjectMethod("setFlags", "(I)V", flag);

	if (intent.isValid())
	{
		qCDebug(qml) << "Call action:" << pAction;
		QtAndroidPrivate::startActivity(intent, 0);
	}

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call an action as activity:" << pAction;
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction) const
{
	const auto& androidQ = QOperatingSystemVersion(QOperatingSystemVersion::Android, 10);

	switch (pAction)
	{
		case Settings::WIFI:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.WIFI"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.WIFI_SETTINGS"));
			}
			break;

		case Settings::NETWORK:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.INTERNET_CONNECTIVITY"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.WIFI_SETTINGS"));
			}
			break;

		case Settings::NFC:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.NFC"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.NFC_SETTINGS"));
			}

			break;

		case Settings::APP:
			qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
			break;
	}
}


bool ApplicationModel::isScreenReaderRunning() const
{
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	const jboolean result = context.callMethod<jboolean>("isScreenReaderRunning", "()Z");
	return result != JNI_FALSE;
}


void ApplicationModel::showFeedback(const QString& pMessage, bool pReplaceExisting)
{
	qCInfo(feedback).noquote() << pMessage;

	QNativeInterface::QAndroidApplication::runOnAndroidMainThread([pMessage, pReplaceExisting](){
				QJniEnvironment env;
				static thread_local QJniObject toast;

				if (toast.isValid() && pReplaceExisting)
				{
					toast.callMethod<void>("cancel");
				}

				QJniObject context = QNativeInterface::QAndroidApplication::context();
				const QJniObject& jMessage = QJniObject::fromString(pMessage);
				toast = QJniObject::callStaticObjectMethod(
						"android/widget/Toast",
						"makeText",
						"(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
						context.object(),
						jMessage.object(),
						jint(1));
				toast.callMethod<void>("show");

				if (env->ExceptionCheck())
				{
					qCCritical(qml) << "Suppressing an unexpected exception.";
					env->ExceptionDescribe();
					env->ExceptionClear();
					// The toast was probably not displayed (e.g. DeadObjectException). We halt on error
					// since it is used to display information to the user as required by the TR.
					Q_ASSERT(false);
				}
			});
}


void ApplicationModel::keepScreenOn(bool pActive) const
{
	QNativeInterface::QAndroidApplication::runOnAndroidMainThread([pActive](){
				QJniObject context = QNativeInterface::QAndroidApplication::context();
				context.callMethod<void>("keepScreenOn", "(Z)V", pActive);
				QJniEnvironment env;
				if (env->ExceptionCheck())
				{
					qCCritical(qml) << "Exception calling java native function.";
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			});

}
