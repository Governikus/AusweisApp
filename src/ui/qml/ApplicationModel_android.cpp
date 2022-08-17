/*!
 * \copyright Copyright (c) 2020-2022 Governikus GmbH & Co. KG, Germany
 */

#include "ApplicationModel.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QOperatingSystemVersion>
#include <QtCore/private/qandroidextras_p.h>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(qml)

static void showSystemSettings(const QString& pAction)
{
	QJniEnvironment env;

	const QJniObject& jAction = QJniObject::fromString(pAction);
	QJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	const jint flag = QJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK");
	intent.callObjectMethod("setFlags", "(I)V", flag);

	if (intent.isValid())
	{
		qCCritical(qml) << "Call action:" << pAction;
		QtAndroidPrivate::startActivity(intent, 0);
	}

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call an action as activity:" << pAction;
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}


void ApplicationModel::showSettings(const ApplicationModel::Settings& pAction)
{
	const auto& androidQ = QOperatingSystemVersion(QOperatingSystemVersion::Android, 10);

	switch (pAction)
	{
		case Settings::SETTING_WIFI:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.WIFI"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.WIFI_SETTINGS"));
			}
			break;

		case Settings::SETTING_NETWORK:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.INTERNET_CONNECTIVITY"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.WIFI_SETTINGS"));
			}
			break;

		case Settings::SETTING_NFC:
			if (QOperatingSystemVersion::current() >= androidQ)
			{
				showSystemSettings(QStringLiteral("android.settings.panel.action.NFC"));
			}
			else
			{
				showSystemSettings(QStringLiteral("android.settings.NFC_SETTINGS"));
			}

			break;

		case Settings::SETTING_APP:
			qCWarning(qml) << "NOT IMPLEMENTED:" << pAction;
			break;
	}
}
