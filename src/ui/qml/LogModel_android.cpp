/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LogHandler.h"

#include <QAndroidJniEnvironment>
#include <QAndroidJniObject>
#include <QLoggingCategory>
#include <QtAndroid>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


/*
 * Calling
 * QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation).last()
 * does the same but we don't want to rely on Qt internals, so we do it on our own.
 */
static QString getPublicLogFileName(QAndroidJniEnvironment& env, const QAndroidJniObject& javaActivity, bool pExternal, const QDateTime& pDateTime = QDateTime::currentDateTime())
{
	QAndroidJniObject jFile;
	if (pExternal)
	{
		const auto& jEmptyString = QAndroidJniObject::fromString(QString());
		jFile = javaActivity.callObjectMethod("getExternalFilesDir", "(Ljava/lang/String;)Ljava/io/File;", jEmptyString.object<jstring>());
	}
	else
	{
		jFile = javaActivity.callObjectMethod("getCacheDir", "()Ljava/io/File;");
	}

	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception determining publicLogFileDir";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return QString();
	}
	if (!jFile.isValid())
	{
		qCCritical(qml) << "Cannot determine publicLogFileDir";
		return QString();
	}

	const auto& jFilesDirPath = jFile.callObjectMethod("getAbsolutePath", "()Ljava/lang/String;");
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception determining publicLogFileDir absolute path";
		env->ExceptionDescribe();
		env->ExceptionClear();
		return QString();
	}
	if (!jFilesDirPath.isValid())
	{
		qCCritical(qml) << "Cannot determine publicLogFileDir absolute path";
		return QString();
	}

	return QStringLiteral("%1/%2").arg(jFilesDirPath.toString(), LogModel::createLogFileName(pDateTime));
}


void LogModel::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	QAndroidJniEnvironment env;
	const QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	const bool external = QAndroidJniObject::callStaticMethod<jboolean>("com/governikus/ausweisapp2/ShareUtil", "isNotAtLeastMarshmallow");

	const auto& jEmail = QAndroidJniObject::fromString(pEmail);
	const auto& jSubject = QAndroidJniObject::fromString(pSubject);
	const auto& jMsg = QAndroidJniObject::fromString(pMsg);
	//: LABEL ANDROID
	const auto& jChooserTitle = QAndroidJniObject::fromString(tr("Send application log per email..."));
	const auto& publicLogFile = getPublicLogFileName(env, javaActivity, external);
	const auto& jPublicLogFile = QAndroidJniObject::fromString(publicLogFile);

	qCDebug(qml) << "Copy logfile to" << publicLogFile;
	if (!Env::getSingleton<LogHandler>()->copy(publicLogFile))
	{
		qCCritical(qml) << "Cannot copy logfile to" << publicLogFile;
		return;
	}

	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
			"mailLog",
			"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jEmail.object<jstring>(),
			jSubject.object<jstring>(),
			jMsg.object<jstring>(),
			jPublicLogFile.object<jstring>(),
			jChooserTitle.object<jstring>());
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception calling ShareUtil.mailLog()";
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}


void LogModel::shareLog(const QPoint /*popupPosition*/)
{
	QAndroidJniEnvironment env;
	const QAndroidJniObject javaActivity(QtAndroid::androidActivity());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	const auto& logHandler = Env::getSingleton<LogHandler>();

	QString publicLogFile;
	if (mSelectedLogFile == 0)
	{
		publicLogFile = getPublicLogFileName(env, javaActivity, false);
		if (!logHandler->copy(publicLogFile))
		{
			qCCritical(qml) << "Cannot copy logfile to" << publicLogFile;
			return;
		}
	}
	else
	{
		const auto& source = mLogFiles.at(mSelectedLogFile);
		const auto& dateTime = logHandler->getFileDate(QFileInfo(source));
		publicLogFile = getPublicLogFileName(env, javaActivity, false, dateTime);
		if (QFile::exists(publicLogFile))
		{
			QFile::remove(publicLogFile);
		}
		if (!QFile::copy(source, publicLogFile))
		{
			qCCritical(qml) << "Cannot copy logfile to" << publicLogFile;
			return;
		}
	}

	//: LABEL ANDROID
	const auto& jChooserTitle = QAndroidJniObject::fromString(tr("Share application log..."));
	const auto& jPublicLogFile = QAndroidJniObject::fromString(publicLogFile);

	QAndroidJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
			"shareLog",
			"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jPublicLogFile.object<jstring>(),
			jChooserTitle.object<jstring>());
	if (env->ExceptionCheck())
	{
		qCCritical(qml) << "Exception calling ShareUtil.shareLog()";
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
}
