/*
 * \copyright Copyright (c) 2018-2021 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "PlatformHelper.h"
#include "Randomizer.h"
#include "SettingsModel.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QtMath>

using namespace governikus;


LogModel::LogModel()
	: QAbstractListModel()
	, mLogFiles()
	, mSelectedLogFile(-1)
	, mLogEntries()
{
	reset();
	connect(Env::getSingleton<SettingsModel>(), &SettingsModel::fireLanguageChanged, this, &LogModel::fireLogFilesChanged); // needed to translate the "Current log" entry on language change
}


void LogModel::reset()
{
	mLogFiles.clear();
	mLogFiles += QString(); // dummy entry for "current logfile"
	const auto logFiles = Env::getSingleton<LogHandler>()->getOtherLogfiles();
	for (const auto& entry : logFiles)
	{
		mLogFiles += entry.absoluteFilePath();
	}

	setLogfile(0);
}


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


QStringList LogModel::getLogfiles() const
{
	QStringList logFileNames;
	//: LABEL ALL_PLATFORMS
	logFileNames += tr("Current log");
	for (const auto& entry : qAsConst(mLogFiles))
	{
		if (!entry.isEmpty())
		{
			logFileNames += LanguageLoader::getInstance().getUsedLocale().toString(LogHandler::getFileDate(QFileInfo(entry)), tr("dd.MM.yyyy hh:mm:ss"));
		}
	}

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
		reset();
		Q_EMIT fireLogFilesChanged();
	}
}


void LogModel::removeCurrentLogfile()
{
	if (mSelectedLogFile == 0)
	{
		return;
	}

	if (!QFile::remove(mLogFiles[mSelectedLogFile]))
	{
		qDebug() << mLogFiles[mSelectedLogFile] << "could not be removed";
		return;
	}

	mLogFiles.removeAt(mSelectedLogFile);
	Q_EMIT fireLogFilesChanged();
}


void LogModel::setLogfile(int pIndex)
{
	if (pIndex < 0 || pIndex >= mLogFiles.size())
	{
		qDebug() << "Called with invalid index:" << pIndex;
		return;
	}

	if (pIndex == mSelectedLogFile)
	{
		return;
	}

	mSelectedLogFile = pIndex;
	const auto logHandler = Env::getSingleton<LogHandler>();

	if (pIndex == 0)
	{
		QTextStream in(logHandler->useLogfile() ? logHandler->getBacklog() : tr("The logfile is disabled.").toUtf8());
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
		in.setCodec("UTF-8");
#endif

		setLogEntries(in);
		connect(logHandler->getEventHandler(), &LogEventHandler::fireLog, this, &LogModel::onNewLogMsg);
	}
	else
	{
		disconnect(logHandler->getEventHandler(), &LogEventHandler::fireLog, this, &LogModel::onNewLogMsg);
		QFile inputFile(mLogFiles[pIndex]);
		if (inputFile.open(QIODevice::ReadOnly))
		{
			QTextStream in(&inputFile);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
			in.setCodec("UTF-8");
#endif
			setLogEntries(in);
			inputFile.close();
		}
	}
}


void LogModel::saveCurrentLogfile(const QUrl& pFilename) const
{
	bool success = false;
	const auto logHandler = Env::getSingleton<LogHandler>();
	const QString logfilePath = mLogFiles.at(mSelectedLogFile);

	if (logfilePath.isEmpty())
	{
		success = logHandler->copy(pFilename.toLocalFile());
	}
	else
	{
		success = logHandler->copyOther(logfilePath, pFilename.toLocalFile());
	}

	const auto applicationModel = Env::getSingleton<ApplicationModel>();
	applicationModel->showFeedback((success ? tr("Successfully saved logfile to \"%1\"") : tr("Error while saving logfile to \"%1\"")).arg(pFilename.toLocalFile()));
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
