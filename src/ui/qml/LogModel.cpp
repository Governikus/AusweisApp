/*
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LanguageLoader.h"
#include "LogHandler.h"
#include "PlatformHelper.h"
#include "Randomizer.h"
#include "SettingsModel.h"
#include "SingletonHelper.h"

#include <QDir>
#include <QFileInfo>
#include <QtMath>


using namespace governikus;


defineSingleton(LogModel)


void LogModel::addLogEntry(const QString& pEntry)
{
	mLogEntries.append(pEntry);
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
	, mSelectedLogFile(-1)
	, mLogEntries()
{
	setLogfile(0);
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
	if (pIndex == mSelectedLogFile)
	{
		return;
	}

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


#ifndef QT_NO_DEBUG
void LogModel::saveDummyLogfile() const
{
	auto& generator = Randomizer::getInstance().getGenerator();
	std::uniform_int_distribution<uint32_t> dist;
	const auto logHandler = Env::getSingleton<LogHandler>();
	logHandler->copy(QDir::temp().filePath(QStringLiteral("%1.%2.log").arg(QCoreApplication::applicationName()).arg(dist(generator))));
}


#endif


int LogModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return mLogEntries.size();
}


QHash<int, QByteArray> LogModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("display"));
	roles.insert(OriginRole, QByteArrayLiteral("origin"));
	roles.insert(MessageRole, QByteArrayLiteral("message"));
	return roles;
}


QVariant LogModel::data(const QModelIndex& pIndex, int pRole) const
{
	static const auto splitToken = QLatin1String(" : ");

	if (!pIndex.isValid())
	{
		return QVariant();
	}

	const auto& logMessage = mLogEntries.at(pIndex.row());

	switch (pRole)
	{
		case Qt::DisplayRole:
			return logMessage;

		case OriginRole:
			return logMessage.section(splitToken, 0, 0).trimmed();

		case MessageRole:
			return logMessage.section(splitToken, 1, -1).trimmed();

		default:
			return QVariant();
	}

	Q_UNREACHABLE();
}


QString LogModel::createLogFileName(const QDateTime& pDateTime)
{
	QString dateFormat = QStringLiteral("yyyy-MM-dd_HH-mm");
	QString logFileDate = pDateTime.toString(dateFormat);
	return QStringLiteral("%1-%2.log").arg(QCoreApplication::applicationName(), logFileDate);
}
