/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#include "LogFilesModel.h"
#include "LanguageLoader.h"
#include "LogHandler.h"
#include "Randomizer.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>


using namespace governikus;


LogFilesModel::LogFilesModel()
	: QAbstractListModel()
	, mLogFiles()
{
	reset();
}


void LogFilesModel::reset()
{
	beginResetModel();

	mLogFiles.clear();
	mLogFiles += QString(); // dummy entry for "current logfile"
	const auto logFiles = Env::getSingleton<LogHandler>()->getOtherLogFiles();
	for (const auto& entry : logFiles)
	{
		mLogFiles += entry.absoluteFilePath();
	}

	endResetModel();
	Q_EMIT fireCountChanged();
}


void LogFilesModel::removeOtherLogFiles()
{
	if (Env::getSingleton<LogHandler>()->removeOtherLogFiles())
	{
		reset();
	}
}


void LogFilesModel::saveDummyLogFile(const QDateTime& pTimestamp)
{
#ifdef QT_NO_DEBUG
	Q_UNUSED(pTimestamp)
#else
	auto* generator = Randomizer::getInstance().getGenerator(false);
	std::uniform_int_distribution<uint32_t> dist;
	const auto* logHandler = Env::getSingleton<LogHandler>();
	const auto& copyFilename = QDir::temp().filePath(QStringLiteral("%1.%2.log").arg(QCoreApplication::applicationName()).arg(dist(*generator)));
	if (logHandler->copy(copyFilename) && pTimestamp.isValid())
	{
		if (QFile file(copyFilename); file.open(QFile::Append))
		{
			file.setFileTime(pTimestamp, QFile::FileModificationTime);
		}
	}
	reset();
#endif
}


QString LogFilesModel::getLogFileName(int pIndex) const
{
	return data(index(pIndex, 0), NameRole).toString();
}


QString LogFilesModel::getLogFilePath(int pIndex) const
{
	return data(index(pIndex, 0), PathRole).toString();
}


int LogFilesModel::getCount() const
{
	return rowCount(QModelIndex());
}


int LogFilesModel::rowCount(const QModelIndex& pIndex) const
{
	Q_UNUSED(pIndex)
	return static_cast<int>(mLogFiles.size());
}


QHash<int, QByteArray> LogFilesModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles.insert(Qt::DisplayRole, QByteArrayLiteral("modelData"));
	roles.insert(NameRole, QByteArrayLiteral("name"));
	roles.insert(PathRole, QByteArrayLiteral("path"));
	return roles;
}


QVariant LogFilesModel::data(const QModelIndex& pIndex, int pRole) const
{
	if (!pIndex.isValid())
	{
		return QVariant();
	}

	const auto& file = mLogFiles.at(pIndex.row());

	switch (pRole)
	{
		case Qt::DisplayRole:
		case NameRole:
		{
			if (file.isEmpty())
			{
				//: ALL_PLATFORMS
				return tr("Current log");
			}

			//: ALL_PLATFORMS Datetime format according to https://doc.qt.io/qt/qdate.html#toString and https://doc.qt.io/qt/qtime.html#toString
			const auto fileName = LanguageLoader::getInstance().getUsedLocale().toString(LogHandler::getFileDate(QFileInfo(file)), tr("dd.MM.yyyy hh:mm:ss"));
			return fileName;
		}

		case PathRole:
		{
			return mLogFiles.at(pIndex.row());
		}

		default:
			return QVariant();
	}

	Q_UNREACHABLE();
}
