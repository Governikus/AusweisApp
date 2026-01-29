/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include <QCoreApplication>
#include <QJniEnvironment>
#include <QJniObject>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QUrl>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


static QString getPublicLogFilePath(const QString& pFileName)
{
	const QStringList cachePaths = QStandardPaths::standardLocations(QStandardPaths::CacheLocation);
	if (cachePaths.isEmpty())
	{
		qCCritical(qml) << "No cache paths found";
		return QString();
	}

	const QString cacheBasePath = cachePaths.first();
	if (cacheBasePath.isEmpty())
	{
		qCCritical(qml) << "Cache base folder is invalid (empty)";
		return QString();
	}

	return QStringLiteral("%1/%2").arg(cacheBasePath, pFileName);
}


void LogModel::mailLogFile(const QString& pEmail, const QString& pSubject, const QString& pMsg) const
{
	QJniEnvironment env;
	const QJniObject javaActivity(QNativeInterface::QAndroidApplication::context());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Can't determine android activity";
		return;
	}

	const auto& publicLogFile = getPublicLogFilePath(createLogFileName());
	if (!saveLogFile(QUrl::fromLocalFile(publicLogFile), false))
	{
		qCCritical(qml) << "Can't copy log file to" << publicLogFile;
		return;
	}

	const auto& jEmail = QJniObject::fromString(pEmail);
	const auto& jSubject = QJniObject::fromString(pSubject);
	const auto& jMsg = QJniObject::fromString(pMsg);
	//: ANDROID
	const auto& jChooserTitle = QJniObject::fromString(tr("Send application log per email..."));
	const auto& jPublicLogFile = QJniObject::fromString(publicLogFile);

	QJniObject::callStaticMethod<void>(
			"com/governikus/ausweisapp2/ShareUtil",
			"mailLog",
			"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jEmail.object<jstring>(),
			jSubject.object<jstring>(),
			jMsg.object<jstring>(),
			jPublicLogFile.object<jstring>(),
			jChooserTitle.object<jstring>());
	if (env.checkAndClearExceptions())
	{
		qCCritical(qml) << "Exception calling ShareUtil.mailLog()";
	}
}


void LogModel::shareLogFile(const QPoint /*popupPosition*/) const
{
	QJniEnvironment env;
	const QJniObject javaActivity(QNativeInterface::QAndroidApplication::context());
	if (!javaActivity.isValid())
	{
		qCCritical(qml) << "Can't determine android activity";
		return;
	}

	const auto& publicLogFile = getPublicLogFilePath(createLogFileName());
	if (!saveLogFile(QUrl::fromLocalFile(publicLogFile), false))
	{
		qCCritical(qml) << "Can't copy log file to" << publicLogFile;
		return;
	}

	//: ANDROID
	const auto& jChooserTitle = QJniObject::fromString(tr("Share application log..."));
	const auto& jPublicLogFile = QJniObject::fromString(publicLogFile);

	QJniObject::callStaticMethod<void>(
			"com/governikus/ausweisapp2/ShareUtil",
			"shareLog",
			"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)V",
			javaActivity.object<jobject>(),
			jPublicLogFile.object<jstring>(),
			jChooserTitle.object<jstring>());
	if (env.checkAndClearExceptions())
	{
		qCCritical(qml) << "Exception calling ShareUtil.shareLog()";
	}
}
