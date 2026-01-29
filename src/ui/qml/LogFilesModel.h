/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QPoint>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QtQml/qqmlregistration.h>


class test_LogFilesModel;

namespace governikus
{

class LogFilesModel
	: public QAbstractListModel
	, public SingletonCreator<LogFilesModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_LogFilesModel;

	Q_PROPERTY(int count READ getCount NOTIFY fireCountChanged)

	private:
		QStringList mLogFiles;

		void reset();

	public:
		enum LogFilesModelRoles
		{
			NameRole = Qt::UserRole + 1,
			PathRole
		};

		LogFilesModel();
		~LogFilesModel() override = default;

		Q_INVOKABLE void removeOtherLogFiles();
		Q_INVOKABLE void saveDummyLogFile(const QDateTime& pTimeStamp = QDateTime());
		[[nodiscard]] Q_INVOKABLE QString getLogFileName(int pIndex) const;
		[[nodiscard]] Q_INVOKABLE QString getLogFilePath(int pIndex) const;
		[[nodiscard]] int getCount() const;

		int rowCount(const QModelIndex& parent) const override;
		QHash<int, QByteArray> roleNames() const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;

	Q_SIGNALS:
		void fireCountChanged();
};


} // namespace governikus
