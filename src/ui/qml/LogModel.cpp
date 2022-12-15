/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "Randomizer.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QtMath>


using namespace governikus;


LogModel::LogModel()
	: QAbstractListModel()
	, mLogFiles()
	, mSelectedLogFile(-1)
	, mLogEntries()
	, mLevels()
	, mCategories()
{
	reset();
}


void LogModel::reset()
{
	mLogFiles.clear();
	mLogFiles += QString(); // dummy entry for "current logfile"
	const auto logFiles = Env::getSingleton<LogHandler>()->getOtherLogFiles();
	for (const auto& entry : logFiles)
	{
		mLogFiles += entry.absoluteFilePath();
	}

	setLogFile(0);
}


void LogModel::addLogEntry(const QString& pEntry)
{
	const QString re = QStringLiteral(R"(^[a-z\._ ]{%1} \d{4}\.\d{2}\.\d{2} \d{2}:\d{2}:\d{2}\.\d{3} \d{3,} )").arg(LogHandler::MAX_CATEGORY_LENGTH);
	if (!QRegularExpression(re).match(pEntry).hasMatch() && !mLogEntries.isEmpty())
	{
		mLogEntries.last().append(QLatin1String("\n")).append(pEntry);
		return;
	}

	mLogEntries.append(pEntry);

	QModelIndex idx = index(mLogEntries.size() - 1, 0);

	const auto& level = data(idx, LogModel::LogModelRoles::LevelRole).toString();
	if (!mLevels.contains(level))
	{
		mLevels.insert(level);
		Q_EMIT fireLevelsChanged();
	}

	const auto& category = data(idx, LogModel::LogModelRoles::CategoryRole).toString();
	if (!mCategories.contains(category))
	{
		mCategories.insert(category);
		Q_EMIT fireCategoriesChanged();
	}
}


void LogModel::setLogEntries(QTextStream& pTextStream)
{
	beginResetModel();

	mLevels.clear();
	mCategories.clear();
	mLogEntries.clear();

	{
		const QSignalBlocker blocker(this);
		while (!pTextStream.atEnd())
		{
			addLogEntry(pTextStream.readLine());
		}
	}

	endResetModel();

	Q_EMIT fireLevelsChanged();
	Q_EMIT fireCategoriesChanged();
}


void LogModel::onNewLogMsg(const QString& pMsg)
{
	if (mSelectedLogFile == 0)
	{
		beginInsertRows(QModelIndex(), mLogEntries.size(), mLogEntries.size());
		addLogEntry(pMsg);
		endInsertRows();
		Q_EMIT fireNewLogMsg();
	}
}


void LogModel::onTranslationChanged()
{
	Q_EMIT fireLogFileNamesChanged();
}


QStringList LogModel::getLogFileNames() const
{
	QStringList logFileNames;
	//: LABEL ALL_PLATFORMS
	logFileNames += tr("Current log");
	for (const auto& entry : std::as_const(mLogFiles))
	{
		if (!entry.isEmpty())
		{
			logFileNames += LanguageLoader::getInstance().getUsedLocale().toString(LogHandler::getFileDate(QFileInfo(entry)), tr("dd.MM.yyyy hh:mm:ss"));
		}
	}

	return logFileNames;
}


const QSet<QString>& LogModel::getLevels() const
{
	return mLevels;
}


const QSet<QString>& LogModel::getCategories() const
{
	return mCategories;
}


QDateTime LogModel::getCurrentLogFileDate() const
{
	if (mSelectedLogFile == 0)
	{
		return Env::getSingleton<LogHandler>()->getCurrentLogFileDate();
	}

	return LogHandler::getFileDate(QFileInfo(mLogFiles.at(mSelectedLogFile)));
}


void LogModel::removeOtherLogFiles()
{
	if (Env::getSingleton<LogHandler>()->removeOtherLogFiles())
	{
		reset();
		Q_EMIT fireLogFileNamesChanged();
	}
}


void LogModel::removeCurrentLogFile()
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
	Q_EMIT fireLogFileNamesChanged();
}


void LogModel::setLogFile(int pIndex)
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
		QTextStream in(logHandler->useLogFile() ? logHandler->getBacklog() : tr("The logfile is disabled.").toUtf8());
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


void LogModel::saveCurrentLogFile(const QUrl& pFilename) const
{
	bool success = false;
	if (const auto& logfilePath = mLogFiles.at(mSelectedLogFile); logfilePath.isEmpty())
	{
		success = Env::getSingleton<LogHandler>()->copy(pFilename.toLocalFile());
	}
	else
	{
		success = Env::getSingleton<LogHandler>()->copyOther(logfilePath, pFilename.toLocalFile());
	}

	const auto applicationModel = Env::getSingleton<ApplicationModel>();
	applicationModel->showFeedback((success ? tr("Successfully saved logfile to \"%1\"") : tr("Error while saving logfile to \"%1\"")).arg(pFilename.toLocalFile()));
}


#ifndef QT_NO_DEBUG
void LogModel::saveDummyLogFile(const QDateTime& pTimestamp)
{
	auto& generator = Randomizer::getInstance().getGenerator();
	std::uniform_int_distribution<uint32_t> dist;
	const auto logHandler = Env::getSingleton<LogHandler>();
	const auto& copyFilename = QDir::temp().filePath(QStringLiteral("%1.%2.log").arg(QCoreApplication::applicationName()).arg(dist(generator)));
	if (logHandler->copy(copyFilename) && pTimestamp.isValid())
	{
		if (QFile file(copyFilename); file.open(QFile::Append))
		{
			file.setFileTime(pTimestamp, QFile::FileModificationTime);
		}
	}
	reset();
	Q_EMIT fireLogFileNamesChanged();
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
	roles.insert(LevelRole, QByteArrayLiteral("level"));
	roles.insert(CategoryRole, QByteArrayLiteral("category"));
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

		case LevelRole:
		{
			const QRegularExpression re(QStringLiteral("[0-9]{3,} ([A-Z]) "));
			const auto& match = re.match(logMessage);
			if (match.hasMatch())
			{
				return match.captured(1);
			}

			return QStringLiteral("D");
		}

		case CategoryRole:
		{
			return logMessage.left(logMessage.indexOf(QLatin1Char(' ')));
		}

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
