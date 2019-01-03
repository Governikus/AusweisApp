/*
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTextStream>

class test_LogModel;

namespace governikus
{

class LogModel
	: public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY(bool autoFlick MEMBER mAutoFlick)
	Q_PROPERTY(QStringList logFiles READ getLogfiles NOTIFY fireLogFilesChanged)
	Q_PROPERTY(double heightRatio READ getSizeRatio NOTIFY fireVisibleAreaChanged)
	Q_PROPERTY(double yPosition READ getPosition WRITE setPosition NOTIFY fireVisibleAreaChanged)

	private:
		friend class ::test_LogModel;
		int mIndex;
		int mCount;
		QStringList mLogFiles;
		int mSelectedLogFile;
		QStringList mLogEntries;
		bool mAutoFlick;

		double getSizeRatio() const;
		double getPosition() const;
		void setPosition(double pPosition);
		void addLogEntry(const QString& mEntry);
		void setLogEntries(QTextStream& pTextStream);

	private Q_SLOTS:
		void onNewLogMsg(const QString& pMsg);

	protected:
		LogModel();

	public:
		static LogModel& getInstance();

		QStringList getLogfiles();
		Q_INVOKABLE void removeOtherLogfiles();
		Q_INVOKABLE void removeCurrentLogfile();
		Q_INVOKABLE void setLogfile(int pIndex);
		Q_INVOKABLE void moveView(int pDistance);

		Q_INVOKABLE void mailLog(const QString& pEmail = tr("support.ausweisapp2@governikus.de"),
				const QString& pSubject = tr("Android log file"),
				const QString& pMsg = tr("<Please describe the error>"));
		Q_INVOKABLE void shareLog();

		int rowCount(const QModelIndex& pIndex = QModelIndex()) const override;
		QVariant data(const QModelIndex& pIndex, int pRole = Qt::DisplayRole) const override;

	Q_SIGNALS:
		void fireVisibleAreaChanged();
		void fireLogFilesChanged();
		void fireNewLogMsg();
};

} // namespace governikus
