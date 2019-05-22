/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "LogModel.h"

#include "LanguageLoader.h"
#include "LogHandler.h"
#include "SingletonHelper.h"

#include <QtMath>


using namespace governikus;


defineSingleton(LogModel)


double LogModel::getSizeRatio() const
{
	if (mLogEntries.isEmpty())
	{
		return 1.0;
	}

	return mCount / static_cast<double>(mLogEntries.size());
}


double LogModel::getPosition() const
{
	if (mLogEntries.isEmpty())
	{
		return 0.0;
	}

	return mIndex / static_cast<double>(mLogEntries.size());
}


void LogModel::setPosition(double pPosition)
{
	beginResetModel();

	const int newIndex = qFloor(mLogEntries.size() * pPosition);
	const int maxIndex = mLogEntries.size() - mCount;
	mIndex = newIndex < maxIndex ? newIndex : maxIndex;

	endResetModel();

	Q_EMIT fireVisibleAreaChanged();
}


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

	mIndex = 0;
	mCount = mLogEntries.size() < 80 ? mLogEntries.size() : 80;

	endResetModel();

	Q_EMIT fireVisibleAreaChanged();
}


void LogModel::onNewLogMsg(const QString& pMsg)
{
	if (mSelectedLogFile == 0)
	{
		const int oldSize = mLogEntries.size();
		const bool flickDown = (mIndex + mCount == oldSize);
		addLogEntry(pMsg);
		Q_EMIT fireVisibleAreaChanged();

		if (flickDown && mAutoFlick)
		{
			moveView(mLogEntries.size() - oldSize);
			Q_EMIT fireNewLogMsg();
		}
	}
}


LogModel::LogModel()
	: QAbstractListModel()
	, mIndex(0)
	, mCount(0)
	, mLogFiles()
	, mSelectedLogFile(0)
	, mLogEntries()
	, mAutoFlick(false)
{
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


void LogModel::moveView(int pDistance)
{
	if (pDistance == 0)
	{
		return;
	}

	int newIndex = mIndex + pDistance;
	if (newIndex < 0)
	{
		newIndex = 0;
	}

	const int maxIndex = mLogEntries.size() - mCount;
	if (newIndex > maxIndex)
	{
		newIndex = maxIndex;
	}

	if (newIndex != mIndex)
	{
		pDistance = newIndex - mIndex;

		if (pDistance < 0)
		{
			if (mCount + pDistance >= 0)
			{
				beginRemoveRows(QModelIndex(), mCount + pDistance, mCount - 1);
				mCount += pDistance;
				endRemoveRows();
			}

			beginInsertRows(QModelIndex(), 0, -pDistance - 1);
			mCount -= pDistance;
			mIndex += pDistance;
			endInsertRows();
		}
		else
		{
			if (mCount - pDistance >= 0)
			{
				beginRemoveRows(QModelIndex(), 0, pDistance - 1);
				mCount -= pDistance;
				mIndex += pDistance;
				endRemoveRows();
			}

			beginInsertRows(QModelIndex(), mCount, mCount + pDistance - 1);
			mCount += pDistance;
			endInsertRows();
		}

		Q_EMIT fireVisibleAreaChanged();
	}
}


int LogModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex);
	return mCount;
}


QVariant LogModel::data(const QModelIndex& pIndex, int pRole) const
{
	Q_UNUSED(pRole);
	return mLogEntries.at(mIndex + pIndex.row());
}
