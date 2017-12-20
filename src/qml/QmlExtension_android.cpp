/*!
 * \copyright Copyright (c) 2016-2017 Governikus GmbH & Co. KG, Germany
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


void QmlExtension::shareText(const QString& pText, const QString& pChooserTitle)
{
	QAndroidJniEnvironment env;
	const QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity.";
		return;
	}

	const QAndroidJniObject& jText = QAndroidJniObject::fromString(pText);
	const QAndroidJniObject& jTitle = QAndroidJniObject::fromString(pChooserTitle);
	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
			"shareText",
			"(Landroid/content/Context;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jText.object<jstring>(),
			jTitle.object<jstring>());

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Cannot call ShareUtil.shareText()";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
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


/*
 * Calling
 * QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).last()
 * does the same but we don't want to rely on Qt internals, so we do it on our own.
 */
QString getPublicLogFileName(QAndroidJniEnvironment& env, const QAndroidJniObject& javaActivity)
{
	const auto& jEmptyString = QAndroidJniObject::fromString(QLatin1String(""));
	const auto& jExternalFilesDir = javaActivity.callObjectMethod("getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;", jEmptyString.object<jstring>());
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception determining externalFilesDir";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return QString();
	}
	if (!jExternalFilesDir.isValid())
	{
		qCCritical(qml) << "Cannot determine externalFilesDir";
		return QString();
	}

	const auto& jExternalFilesDirPath = jExternalFilesDir.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;");
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception determining externalFilesDir absolute path";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return QString();
	}
	if (!jExternalFilesDirPath.isValid())
	{
		qCCritical(qml) << "Cannot determine externalFilesDir absolute path";
		return QString();
	}

	const auto& nowAsISO = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd'T'HH:mm:ss-t"));
	return QStringLiteral("%1/AusweisApp2-%2.log").arg(jExternalFilesDirPath.toString(), nowAsISO);
}


void QmlExtension::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	QAndroidJniEnvironment env;
	const QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	const auto& jEmail = QAndroidJniObject::fromString(pEmail);
	const auto& jSubject = QAndroidJniObject::fromString(pSubject);
	const auto& jMsg = QAndroidJniObject::fromString(pMsg);
	const auto& jChooserTitle = QAndroidJniObject::fromString(tr("Send application log per email..."));
	const auto& publicLogFile = getPublicLogFileName(env, javaActivity);
	const auto& jPublicLogFile = QAndroidJniObject::fromString(publicLogFile);

	qCDebug(qml) << "Copy log file to" << publicLogFile;
	if (!LogHandler::getInstance().copy(publicLogFile))
	{
		qCCritical(qml) << "Cannot copy log file to" << publicLogFile;
		return;
	}

	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
			"shareLog",
			"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jEmail.object<jstring>(),
			jSubject.object<jstring>(),
			jMsg.object<jstring>(),
			jPublicLogFile.object<jstring>(),
			jChooserTitle.object<jstring>());
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception calling ShareUtil.shareLog()";
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
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


#include "moc_QmlExtension.cpp"
