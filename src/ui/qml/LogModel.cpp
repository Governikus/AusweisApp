/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "ApplicationModel.h"
#include "LogHandler.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QSignalBlocker>
#include <QtMath>


Q_DECLARE_LOGGING_CATEGORY(qml)


using namespace governikus;


LogModel::LogModel()
	: QAbstractListModel()
	, mLogEntries()
	, mLevels()
	, mCategories()
	, mLogFilePath(QStringLiteral("uninitialized"))
{
	// initial current log is used
	setSource(QString());
}


void LogModel::addLogEntry(const QString& pEntry)
{
	const QString re = QStringLiteral(R"(^[a-z\._ ]{%1} \d{4}\.\d{2}\.\d{2} \d{2}:\d{2}:\d{2}\.\d{3} \d{3,} )").arg(LogHandler::MAX_CATEGORY_LENGTH);
	if (!QRegularExpression(re).match(pEntry).hasMatch() && !mLogEntries.isEmpty())
	{
		mLogEntries.last().append(QLatin1String("\n")).append(pEntry); // clazy:exclude=detaching-member
		return;
	}

	mLogEntries.append(pEntry);

	QModelIndex idx = QAbstractListModel::index(static_cast<int>(mLogEntries.size()) - 1, 0);

	const auto& level = LogModel::data(idx, LogModel::LogModelRoles::LevelRole).toString();
	if (!mLevels.contains(level))
	{
		mLevels.insert(level);
		Q_EMIT fireLevelsChanged();
	}

	const auto& category = LogModel::data(idx, LogModel::LogModelRoles::CategoryRole).toString();
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


bool LogModel::isCurrentLog() const
{
	return mLogFilePath.isEmpty();
}


void LogModel::onNewLogMsg(const QString& pMsg)
{
	if (isCurrentLog())
	{
		const auto size = static_cast<int>(mLogEntries.size());
		beginInsertRows(QModelIndex(), size, size);
		addLogEntry(pMsg);
		endInsertRows();
		Q_EMIT fireNewLogMsg();
	}
}


const QSet<QString>& LogModel::getLevels() const
{
	return mLevels;
}


const QSet<QString>& LogModel::getCategories() const
{
	return mCategories;
}


void LogModel::setSource(const QString& pLogFilePath)
{
	if (mLogFilePath == pLogFilePath)
	{
		return;
	}

	mLogFilePath = pLogFilePath;

	auto* logHandler = Env::getSingleton<LogHandler>();

	if (isCurrentLog())
	{
		QTextStream in(logHandler->useLogFile() ? logHandler->getBacklog() : tr("The logfile is disabled.").toUtf8());
		setLogEntries(in);
		connect(logHandler->getEventHandler(), &LogEventHandler::fireLog, this, &LogModel::onNewLogMsg);
	}
	else
	{
		disconnect(logHandler->getEventHandler(), &LogEventHandler::fireLog, this, &LogModel::onNewLogMsg);
		QFile inputFile(mLogFilePath);
		if (inputFile.open(QIODevice::ReadOnly))
		{
			qCDebug(qml) << "Loaded log:" << pLogFilePath;
			QTextStream in(&inputFile);
			setLogEntries(in);
			inputFile.close();
		}
		else
		{
			qCWarning(qml) << "Load file failed:" << pLogFilePath;
			QString emptyString;
			QTextStream stream(&emptyString);
			setLogEntries(stream);
		}
	}

	Q_EMIT fireSourceChanged();
}


[[nodiscard]] QString LogModel::getSource() const
{
	return mLogFilePath;
}


bool LogModel::saveLogFile(const QUrl& pFilename, bool pShowFeedback) const
{
	bool success = false;
	if (isCurrentLog())
	{
		success = Env::getSingleton<LogHandler>()->copy(pFilename.toLocalFile());
	}
	else
	{
		success = Env::getSingleton<LogHandler>()->copyOther(mLogFilePath, pFilename.toLocalFile());
	}

	if (pShowFeedback && !success)
	{
		auto* applicationModel = Env::getSingleton<ApplicationModel>();
		applicationModel->showFeedback(tr("Error while saving logfile to \"%1\"").arg(pFilename.toLocalFile()));
	}

	return success;
}


QString LogModel::createLogFileName() const
{
	const QDateTime dateTime = isCurrentLog()
		? Env::getSingleton<LogHandler>()->getCurrentLogFileDate()
		: LogHandler::getFileDate(QFileInfo(mLogFilePath));

	auto dateFormat = QStringLiteral("yyyy-MM-dd_HH-mm");
	const QString logFileDate = dateTime.toString(dateFormat);
	return QStringLiteral("%1-%2.log").arg(QCoreApplication::applicationName(), logFileDate);
}


int LogModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return static_cast<int>(mLogEntries.size());
}


QHash<int, QByteArray> LogModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("modelData"));
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
