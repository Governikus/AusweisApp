/**
 * Copyright (c) 2018-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

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
{
	Q_OBJECT
	QML_ELEMENT

	friend class Env;
	friend class ::test_LogModel;

	Q_PROPERTY(QString source READ getSource WRITE setSource NOTIFY fireSourceChanged)

	private:
		QStringList mLogEntries;
		QSet<QString> mLevels;
		QSet<QString> mCategories;
		QString mLogFilePath;

		void addLogEntry(const QString& pEntry);
		void setLogEntries(QTextStream& pTextStream);
		[[nodiscard]] bool isCurrentLog() const;

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg);

	public:
		enum LogModelRoles
		{
			OriginRole = Qt::UserRole + 1,
			LevelRole,
			CategoryRole,
			MessageRole
		};

		LogModel();
		~LogModel() override = default;

		[[nodiscard]] const QSet<QString>& getLevels() const;
		[[nodiscard]] const QSet<QString>& getCategories() const;
		void setSource(const QString& pLogFilePath);
		[[nodiscard]] QString getSource() const;
		[[nodiscard]] Q_INVOKABLE bool saveLogFile(const QUrl& pFilename, bool pShowFeedback) const;
		Q_INVOKABLE void mailLogFile(
			const QString& pEmail = QStringLiteral("support@ausweisapp.de"),
			const QString& pSubject = tr("Mobile logfile"),
			const QString& pMsg = tr("<Please describe the error>")) const;
		// \a popupPosition will be used on an iPad as the origin of the share bubble
		Q_INVOKABLE void shareLogFile(const QPoint popupPosition) const;
		[[nodiscard]] Q_INVOKABLE QString createLogFileName() const;

		int rowCount(const QModelIndex& pIndex) const override;
		QHash<int, QByteArray> roleNames() const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;

	Q_SIGNALS:
		void fireLevelsChanged();
		void fireCategoriesChanged();
		void fireNewLogMsg();
		void fireSourceChanged();
};


} // namespace governikus
