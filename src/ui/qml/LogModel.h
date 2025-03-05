/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"
#include "SingletonCreator.h"

#include <QAbstractListModel>
#include <QDateTime>
#include <QObject>
#include <QPoint>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUrl>
#include <QtQml/qqmlregistration.h>

class test_LogModel;

namespace governikus
{

class LogModel
	: public QAbstractListModel
	, public SingletonCreator<LogModel>
{
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON

	friend class Env;
	friend class ::test_LogModel;

	Q_PROPERTY(QStringList logFileNames READ getLogFileNames NOTIFY fireLogFileNamesChanged)

	private:
		QStringList mLogFiles;
		int mSelectedLogFile;
		QStringList mLogEntries;

		QSet<QString> mLevels;
		QSet<QString> mCategories;

		LogModel();
		~LogModel() override = default;

		void reset();
		void addLogEntry(const QString& pEntry);
		void setLogEntries(QTextStream& pTextStream);

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg);

	public Q_SLOTS:
		void onTranslationChanged();

	public:
		enum LogModelRoles
		{
			OriginRole = Qt::UserRole + 1,
			LevelRole,
			CategoryRole,
			MessageRole
		};

		QStringList getLogFileNames() const;
		[[nodiscard]] const QSet<QString>& getLevels() const;
		[[nodiscard]] const QSet<QString>& getCategories() const;
		Q_INVOKABLE QDateTime getCurrentLogFileDate() const;
		Q_INVOKABLE void removeOtherLogFiles();
		Q_INVOKABLE void setLogFile(int pIndex);
		Q_INVOKABLE void saveCurrentLogFile(const QUrl& pFilename) const;
		Q_INVOKABLE void saveDummyLogFile(const QDateTime& pTimeStamp = QDateTime());
		Q_INVOKABLE void mailLog(const QString& pEmail = QStringLiteral("support@ausweisapp.de"),
				const QString& pSubject = tr("Mobile logfile"),
				const QString& pMsg = tr("<Please describe the error>")) const;

		// \a popupPosition will be used on an iPad as the origin of the share bubble
		Q_INVOKABLE void shareLog(QPoint popupPosition) const;

		int rowCount(const QModelIndex& pIndex = QModelIndex()) const override;
		QHash<int, QByteArray> roleNames() const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;
		Q_INVOKABLE static QString createLogFileName(const QDateTime& pDateTime = QDateTime::currentDateTime());

	Q_SIGNALS:
		void fireLogFileNamesChanged();
		void fireLevelsChanged();
		void fireCategoriesChanged();
		void fireNewLogMsg();
};

} // namespace governikus
