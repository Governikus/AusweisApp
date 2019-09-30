/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LanguageLoader.h"
#include "LogHandler.h"
#include "PlatformHelper.h"
#include "SettingsModel.h"
#include "SingletonHelper.h"

#include <QFileInfo>
#include <QtMath>


using namespace governikus;


defineSingleton(LogModel)


void LogModel::addLogEntry(const QString& mEntry)
{
	const int cutIndex = mEntry.indexOf(QLatin1String(" : "));
	if (cutIndex >= 0)
	{
		mLogEntries += mEntry.mid(0, cutIndex).trimmed();
		mLogEntries += mEntry.mid(cutIndex + 3).trimmed();
	}
	else
	{
		mLogEntries += mEntry;
		mLogEntries += QString();
	}
}


void LogModel::setLogEntries(QTextStream& pTextStream)
{
	beginResetModel();

	mLogEntries.clear();
	while (!pTextStream.atEnd())
	{
		addLogEntry(pTextStream.readLine());
	}

	endResetModel();
}


void LogModel::onNewLogMsg(const QString& pMsg)
{
	if (mSelectedLogFile == 0)
	{
		const int oldSize = mLogEntries.size();
		addLogEntry(pMsg);
		beginInsertRows(QModelIndex(), oldSize, mLogEntries.size() - 1);
		endInsertRows();
		Q_EMIT fireNewLogMsg();
	}
}


LogModel::LogModel()
	: QAbstractListModel()
	, mLogFiles()
	, mSelectedLogFile(0)
	, mLogEntries()
{
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireLanguageChanged, this, &LogModel::fireLogFilesChanged); // needed to translate the "Current log" entry on language change
}


LogModel& LogModel::getInstance()
{
	return *Instance;
}


QStringList LogModel::getLogfiles()
{
	mLogFiles.clear();
	mLogFiles += QString();
	const auto& logHandler = Env::getSingleton<LogHandler>();

	QStringList logFileNames;
	//: LABEL ALL_PLATFORMS
	logFileNames += tr("Current log");
	const auto& logFiles = logHandler->getOtherLogfiles();
	for (const auto& entry : logFiles)
	{
		mLogFiles += entry.absoluteFilePath();
		logFileNames += LanguageLoader::getInstance().getUsedLocale().toString(logHandler->getFileDate(entry), tr("dd.MM.yyyy hh:mm:ss"));
	}

	setLogfile(0);
	return logFileNames;
}


QDateTime LogModel::getCurrentLogfileDate() const
{
	if (mSelectedLogFile == 0)
	{
		return Env::getSingleton<LogHandler>()->getCurrentLogfileDate();
	}

	return LogHandler::getFileDate(QFileInfo(mLogFiles.at(mSelectedLogFile)));
}


void LogModel::removeOtherLogfiles()
{
	if (Env::getSingleton<LogHandler>()->removeOtherLogfiles())
	{
		Q_EMIT fireLogFilesChanged();
	}
}


void LogModel::removeCurrentLogfile()
{
	QFile::remove(mLogFiles[mSelectedLogFile]);
	Q_EMIT fireLogFilesChanged();
}


void LogModel::setLogfile(int pIndex)
{
	mSelectedLogFile = pIndex;
	const auto& logHandler = Env::getSingleton<LogHandler>();

	if (pIndex == 0)
	{
		const auto& backLog = logHandler->getBacklog();
		QTextStream in(backLog);
		setLogEntries(in);
		connect(logHandler, &LogHandler::fireLog, this, &LogModel::onNewLogMsg);
	}
	else
	{
		disconnect(logHandler, &LogHandler::fireLog, this, &LogModel::onNewLogMsg);
		QFile inputFile(mLogFiles[pIndex]);
		if (inputFile.open(QIODevice::ReadOnly))
		{
			QTextStream in(&inputFile);
			setLogEntries(in);
			inputFile.close();
		}
	}
}


void LogModel::saveCurrentLogfile(const QUrl& pFilename) const
{
	const QString logfilePath = mLogFiles.at(mSelectedLogFile);

	if (logfilePath.isEmpty())
	{
		const auto& logHandler = Env::getSingleton<LogHandler>();
		logHandler->copy(pFilename.toLocalFile());
	}
	else
	{
		QFile::copy(logfilePath, pFilename.toLocalFile());
	}
}


int LogModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return mLogEntries.size();
}


QVariant LogModel::data(const QModelIndex& pIndex, int pRole) const
{
	Q_UNUSED(pRole)
	return mLogEntries.at(pIndex.row());
}


QString LogModel::createLogFileName(const QDateTime& pDateTime)
{
	QString dateFormat = QStringLiteral("yyyy-MM-dd_HH-mm");
	QString logFileDate = pDateTime.toString(dateFormat);
	return QStringLiteral("%1-%2.log").arg(QCoreApplication::applicationName(), logFileDate);
}
