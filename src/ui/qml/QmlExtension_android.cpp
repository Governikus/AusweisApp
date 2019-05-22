/*!
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#include "QmlExtension.h"

#include "LogHandler.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QtAndroid>

Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;

void QmlExtension::showSettings(const QString& pAction)
{
	QAndroidJniEnvironment env;

	const QAndroidJniObject& jAction = QAndroidJniObject::fromString(pAction);
	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	const jint flag = QAndroidJniObject::getStaticField<jint>("android/content/Intent", "FLAG_ACTIVITY_NEW_TASK");
	intent.callObjectMethod("setFlags", "(I)V", flag);

	if (intent.isValid())
	{
		qCCritical(qml) << "Call action:" << pAction;
		QtAndroid::startActivity(intent, 0);
	}

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call an action as activity:" << pAction;
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}


void QmlExtension::showFeedback(const QString& pMessage)
{
	// Wait for toast activation synchronously so that the app can not be deactivated
	// in the meantime and all used Java objects are still alive when accessed.
	QtAndroid::runOnAndroidThreadSync([pMessage](){
				QAndroidJniEnvironment env;

				const QAndroidJniObject& jMessage = QAndroidJniObject::fromString(pMessage);
				const QAndroidJniObject& toast = QAndroidJniObject::callStaticObjectMethod(
						"android/widget/Toast",
						"makeText",
						"(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
						QtAndroid::androidActivity().object(),
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


bool QmlExtension::exportHistory(const QString&) const
{
	qCWarning(qml) << "NOT IMPLEMENTED";
	return false;
}


void QmlExtension::keepScreenOn(bool pActive)
{
	QtAndroid::runOnAndroidThread([pActive](){
				QtAndroid::androidActivity().callMethod<void>("keepScreenOn", "(Z)V", pActive);
				QAndroidJniEnvironment env;
				if (env->ExceptionCheck())
				{
					qCCritical(qml) << "Exception calling java native function.";
					env->ExceptionDescribe();
					env->ExceptionClear();
				}
			});
}


void QmlExtension::openOnlineHelp(const QString& pHelpSectionName)
{
	Q_UNUSED(pHelpSectionName);
	qCWarning(qml) << "NOT IMPLEMENTED";
}


#include "moc_QmlExtension.cpp"
