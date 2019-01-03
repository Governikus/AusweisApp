/*
 * \brief Logging handler of QtMessageHandler
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "Env.h"

#include <QDateTime>
#include <QDebug>
#include <QFileInfoList>
#include <QLoggingCategory>
#include <QMessageLogContext>
#include <QMutex>
#include <QStringList>
#include <QTemporaryFile>

#define spawnMessageLogger(category)\
	QMessageLogger(QT_MESSAGELOG_FILE, QT_MESSAGELOG_LINE, QT_MESSAGELOG_FUNC, category().categoryName())

class test_LogHandler;

namespace governikus
{

class LogHandler
	: public QObject
{
	Q_OBJECT
	friend class Env;
	friend class ::test_LogHandler;

	private:
		static QString getLogFileTemplate();

		const bool mEnvPattern;
		const int mFunctionFilenameSize;
		qint64 mBacklogPosition;
		const QString mMessagePattern, mDefaultMessagePattern;
		QTemporaryFile mLogFile;
		QtMessageHandler mHandler;
		const QByteArray mFilePrefix;
		QMutex mMutex;

		inline void copyMessageLogContext(const QMessageLogContext& pSource, QMessageLogContext& pDestination, const QByteArray& pFilename = QByteArray(), const QByteArray& pFunction = QByteArray(), const QByteArray& pCategory = QByteArray());
		inline void logToFile(const QString& pOutput);
		inline QByteArray formatFunction(const char* pFunction, const QByteArray& pFilename, int pLine) const;
		inline QByteArray formatFilename(const char* pFilename) const;
		inline QByteArray formatCategory(const QByteArray& pCategory) const;

		QString getPaddedLogMsg(const QMessageLogContext& pContext, const QString& pMsg);
		void handleMessage(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg);
		void removeOldLogfiles();

		static void messageHandler(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg);
		friend QDebug operator<<(QDebug, const LogHandler&);

	protected:
		LogHandler();
		virtual ~LogHandler();
		static LogHandler& getInstance();

#ifndef QT_NO_DEBUG

	public:
#endif
		void reset();
		bool isInitialized() const;

	public:
		void init();

		void setAutoRemove(bool pRemove);
		bool copy(const QString& pDest);
		void resetBacklog();
		QByteArray getBacklog();

		static QDateTime getFileDate(const QFileInfo& pInfo);
		QDateTime getCurrentLogfileDate() const;
		QFileInfoList getOtherLogfiles() const;
		bool removeOtherLogfiles();
		void setLogfile(bool pEnable);
		bool useLogfile() const;

	Q_SIGNALS:
		/**
		 * \brief Every log will be fired by this signal. Be aware that you NEVER use a qDebug()
		 * or something like that function in your slot or you will get a deadlock!
		 */
		void fireLog(const QString& pMsg);
		void fireRawLog(const QString& pMsg, const QString& pCategoryName);
};

inline QDebug operator<<(QDebug pDbg, const governikus::LogHandler& pHandler)
{
	QDebugStateSaver saver(pDbg);
	pDbg.nospace() << pHandler.mLogFile.fileName();
	return pDbg.space();
}


} // namespace governikus
