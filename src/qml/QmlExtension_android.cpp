/*!
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
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

	QAndroidJniObject jAction = QAndroidJniObject::fromString(pAction);
	QAndroidJniObject intent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	jint flag = QAndroidJniObject::getStaticField<jint>(
			"android/content/Intent",
			"FLAG_ACTIVITY_NEW_TASK");
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
	QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (javaActivity == nullptr)
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	QAndroidJniObject jText = QAndroidJniObject::fromString(pText);
	QAndroidJniObject jTitle = QAndroidJniObject::fromString(pChooserTitle);
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
	QtAndroid::runOnAndroidThread([pMessage] {
				const QAndroidJniObject javaString = QAndroidJniObject::fromString(pMessage);
			    // Using the appcontext is essential since it holds the default application colours
				const QAndroidJniObject androidAppContext = QtAndroid::androidActivity().callObjectMethod(
						"getApplicationContext",
						"()Landroid/content/Context;");
				QAndroidJniObject toast = QAndroidJniObject::callStaticObjectMethod(
						"android/widget/Toast",
						"makeText",
						"(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;",
						androidAppContext.object(),
						javaString.object(),
						jint(1));
				toast.callMethod<void>("show");
			});
}


/*
 * Calling
 * QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).last()
 * does the same but we don't want to rely on Qt internals, so we do it on our own.
 */
QString getPublicLogFileName(QAndroidJniEnvironment& env, const QAndroidJniObject& javaActivity)
{
	const auto jEmptyString = QAndroidJniObject::fromString(QLatin1String(""));
	const auto& jExternalFilesDir = javaActivity.callObjectMethod("getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;", jEmptyString.object<jstring>());
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception determining externalFilesDir";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return QString();
	}
	if (jExternalFilesDir == nullptr)
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
	if (jExternalFilesDirPath == nullptr)
	{
		qCCritical(qml) << "Cannot determine externalFilesDir absolute path";
		return QString();
	}

	const auto nowAsISO = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd'T'HH:mm:ss-t"));
	return QStringLiteral("%1/AusweisApp2-%2.log").arg(jExternalFilesDirPath.toString(), nowAsISO);
}


void QmlExtension::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	QAndroidJniEnvironment env;
	QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (javaActivity == nullptr)
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
	qCWarning(qml) << "NOT IMPLEMENTED YET";
	return false;
}


#include "moc_QmlExtension.cpp"
