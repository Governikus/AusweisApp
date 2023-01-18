/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LogHandler.h"

#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>
#include <QStandardPaths>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


static QString getPublicLogFileName(const QDateTime& pDateTime = QDateTime::currentDateTime())
{
	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if (cachePaths.isEmpty())
	{
		qCCritical(qml) << "No cache paths found!";
		return QString();
	}

	const QString cacheBasePath = cachePaths.first();
	if (cacheBasePath.isEmpty())
	{
		qCCritical(qml) << "Cache base folder is invalid (empty).";
		return QString();
	}

	return QStringLiteral("%1/%2").arg(cacheBasePath, LogModel::createLogFileName(pDateTime));
}


void LogModel::mailLog(const QString& pEmail, const QString& pSubject, const QString& pMsg)
{
	QJniEnvironment env;
	const QJniObject javaActivity(QNativeInterface::QAndroidApplication::context());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	const auto& jEmail = QJniObject::fromString(pEmail);
	const auto& jSubject = QJniObject::fromString(pSubject);
	const auto& jMsg = QJniObject::fromString(pMsg);
	//: LABEL ANDROID
	const auto& jChooserTitle = QJniObject::fromString(tr("Send application log per email..."));
	const auto& publicLogFile = getPublicLogFileName();
	const auto& jPublicLogFile = QJniObject::fromString(publicLogFile);

	qCDebug(qml) << "Copy logfile to" << publicLogFile;
	if (!Env::getSingleton<LogHandler>()->copy(publicLogFile))
	{
		qCCritical(qml) << "Cannot copy logfile to" << publicLogFile;
		return;
	}

	QJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
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
	QJniEnvironment env;
	const QJniObject javaActivity(QNativeInterface::QAndroidApplication::context());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Cannot determine android activity";
		return;
	}

	const auto& logHandler = Env::getSingleton<LogHandler>();

	QString publicLogFile;
	if (mSelectedLogFile == 0)
	{
		publicLogFile = getPublicLogFileName();
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
		publicLogFile = getPublicLogFileName(dateTime);
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
	const auto& jChooserTitle = QJniObject::fromString(tr("Share application log..."));
	const auto& jPublicLogFile = QJniObject::fromString(publicLogFile);

	QJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/ShareUtil",
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
