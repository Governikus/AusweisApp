/*
 * \brief Logging handler of QtMessageHandler
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include <QDebug>
#include <QFileInfoList>
#include <QMessageLogContext>
#include <QMutex>
#include <QStringList>
#include <QTemporaryFile>

class test_LogHandler;

namespace governikus
{

class LogHandler
	: public QObject
{
	Q_OBJECT
	friend class::test_LogHandler;

	private:
		const bool mEnvPattern;
		const int mFunctionFilenameSize;
		qint64 mBacklogPosition;
		const QString mDefaultMessagePattern;
		QTemporaryFile mLogFile;
		QtMessageHandler mHandler;
		const QByteArray mFilePrefix;
		QMutex mMutex;

		inline void copyMessageLogContext(const QMessageLogContext& pSource, QMessageLogContext& pDestination, const QByteArray& pFilename = QByteArray(), const QByteArray& pFunction = QByteArray(), const QByteArray& pCategory = QByteArray());
		inline void logToFile(const QString& pOutput);
		inline QByteArray formatFunction(const char* pFunction, const QByteArray& pFilename, int pLine);
		inline QByteArray formatFilename(const char* pFilename);
		inline QByteArray formatCategory(const char* pCategory);

		QString getPaddedLogMsg(const QMessageLogContext& pContext, const QString& pMsg);
		void handleMessage(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg);

		static void messageHandler(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg);
		friend QDebug operator<<(QDebug, const LogHandler&);

	protected:
		LogHandler();
		virtual ~LogHandler();

	public:
		static LogHandler& getInstance();
		void init();

		void setAutoRemove(bool pRemove);
		bool copy(const QString& pDest);
		void resetBacklog();
		QByteArray getBacklog();

		QFileInfoList getOtherLogfiles() const;
		void removeOtherLogfiles();

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


} /* namespace governikus */
