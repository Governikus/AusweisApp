/*
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "LogHandler.h"

#include "SingletonHelper.h"

#include <QDateTime>
#include <QDir>

using namespace governikus;

defineSingleton(LogHandler)


#if !defined(Q_OS_ANDROID) && !defined(QT_USE_JOURNALD)
#define ENABLE_MESSAGE_PATTERN
#endif


LogHandler::LogHandler()
	: QObject()
	, mEnvPattern(!qEnvironmentVariableIsEmpty("QT_MESSAGE_PATTERN"))
	, mFunctionFilenameSize(74)
	, mBacklogPosition(0)
	, mMessagePattern(QStringLiteral("%{category} %{time yyyy.MM.dd hh:mm:ss.zzz} %{if-debug}D%{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{function}(%{file}:%{line}) %{message}"))
	, mDefaultMessagePattern(QStringLiteral("%{if-category}%{category}: %{endif}%{message}")) // as defined in qlogging.cpp
	, mLogFile(QDir::tempPath() + QStringLiteral("/AusweisApp2.XXXXXX.log")) // if you change value you need to adjust getOtherLogfiles()
	, mHandler(nullptr)
	, mFilePrefix("/src/")
	, mMutex()
{
	mLogFile.open();
}


LogHandler::~LogHandler()
{
	const QMutexLocker mutexLocker(&mMutex);
	if (mHandler)
	{
		qInstallMessageHandler(nullptr);
		mHandler = nullptr;
	}
}


LogHandler& LogHandler::getInstance()
{
	return *Instance;
}


void LogHandler::init()
{
	const QMutexLocker mutexLocker(&mMutex);
	if (!mHandler)
	{
		mHandler = qInstallMessageHandler(&LogHandler::messageHandler);
	}
}


void LogHandler::setAutoRemove(bool pRemove)
{
	mLogFile.setAutoRemove(pRemove);
}


void LogHandler::logToFile(const QString& pOutput)
{
	if (mLogFile.isOpen() && mLogFile.isWritable())
	{
		mLogFile.write(pOutput.toUtf8());
		mLogFile.flush();
	}
}


QByteArray LogHandler::getBacklog()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (mLogFile.isOpen() && mLogFile.isReadable())
	{
		const auto currentPos = mLogFile.pos();
		mLogFile.seek(mBacklogPosition);
		const auto backlog = mLogFile.readAll();
		mLogFile.seek(currentPos);
		return backlog;
	}

	return tr("An error occurred in log handling: %1").arg(mLogFile.errorString()).toUtf8();
}


void LogHandler::resetBacklog()
{
	const QMutexLocker mutexLocker(&mMutex);
	mBacklogPosition = mLogFile.pos();
}


void LogHandler::copyMessageLogContext(const QMessageLogContext& pSource, QMessageLogContext& pDestination, const QByteArray& pFilename, const QByteArray& pFunction, const QByteArray& pCategory)
{
	pDestination.file = pFilename.isNull() ? pSource.file : pFilename.constData();
	pDestination.function = pFunction.isNull() ? pSource.function : pFunction.constData();
	pDestination.category = pCategory.isNull() ? pSource.category : pCategory.constData();

	pDestination.line = pSource.line;
	pDestination.version = pSource.version;
}


QByteArray LogHandler::formatFilename(const char* pFilename)
{
	QByteArray filename(pFilename);

	// Normalize the file name
	filename.replace(QByteArrayLiteral("\\"), "/");

	// Remove useless path
	return filename.mid(filename.lastIndexOf(mFilePrefix) + mFilePrefix.size());
}


QByteArray LogHandler::formatFunction(const char* pFunction, const QByteArray& pFilename, int pLine)
{
	QByteArray function(pFunction);

	// Remove namespace governikus::
	function.replace(QByteArrayLiteral("governikus::"), "");

	// Remove the parameter list
	const auto start = function.indexOf('(');
	const auto end = function.indexOf(')');
	function = function.left(start) + function.mid(end + 1);

	// Remove the return type (if any)
	if (function.indexOf(' ') != -1)
	{
		function = function.mid(function.lastIndexOf(' ') + 1);
	}

	// Trim function name
	const int size = mFunctionFilenameSize - 3 - pFilename.size() - QString::number(pLine).size();

	if (size >= function.size())
	{
		return function;
	}

	if (size < 3)
	{
		return QByteArray();
	}

	if (size < 10)
	{
		return QByteArrayLiteral("...");
	}

	return QByteArrayLiteral("...") + function.right(size - 3);
}


QByteArray LogHandler::formatCategory(const char* pCategory)
{
	static const char* PADDING = "          ";
	static const int MAX_LENGTH = static_cast<int>(qstrlen(PADDING));

	QByteArray category(pCategory);
	if (category.length() > MAX_LENGTH)
	{
		category = category.left(MAX_LENGTH - 3) + QByteArrayLiteral("...");
	}
	return category.append(PADDING, MAX_LENGTH - category.size());
}


QString LogHandler::getPaddedLogMsg(const QMessageLogContext& pContext, const QString& pMsg)
{
	const int paddingSize = (pContext.function == nullptr && pContext.file == nullptr && pContext.line == 0) ?
			mFunctionFilenameSize - 18 : // padding for nullptr == "unknown(unknown:0)"
			mFunctionFilenameSize - 3 - static_cast<int>(qstrlen(pContext.function)) - static_cast<int>(qstrlen(pContext.file)) - QString::number(pContext.line).size();

	QString padding;
	padding.reserve(paddingSize + pMsg.size() + 3);
	padding.fill(QLatin1Char(' '), paddingSize);
	padding += QStringLiteral(": ");
	padding += pMsg;
	return padding;
}


void LogHandler::handleMessage(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg)
{
	const QMutexLocker mutexLocker(&mMutex);

	QByteArray filename = formatFilename(pContext.file);
	QByteArray function = formatFunction(pContext.function, filename, pContext.line);
	QByteArray category = formatCategory(pContext.category);

	QMessageLogContext ctx;
	copyMessageLogContext(pContext, ctx, filename, function, category);

	const QString& message = mEnvPattern ? pMsg : getPaddedLogMsg(ctx, pMsg);

	qSetMessagePattern(mMessagePattern);
	QString logMsg = qFormatLogMessage(pType, ctx, message) + QLatin1Char('\n');
	logToFile(logMsg);

#ifdef ENABLE_MESSAGE_PATTERN
	mHandler(pType, ctx, message);
#else
	qSetMessagePattern(mDefaultMessagePattern);
	mHandler(pType, ctx, pMsg);
#endif

	Q_EMIT fireRawLog(pMsg, QString::fromLatin1(pContext.category));
	Q_EMIT fireLog(logMsg);
}


bool LogHandler::copy(const QString& pDest)
{
	if (pDest.trimmed().isEmpty())
	{
		return false;
	}

	const QMutexLocker mutexLocker(&mMutex);
	return QFile::copy(mLogFile.fileName(), pDest);
}


QFileInfoList LogHandler::getOtherLogfiles() const
{
	QDir tmpPath = QDir::temp();
	tmpPath.setFilter(QDir::Files);
	tmpPath.setNameFilters(QStringList({QStringLiteral("AusweisApp2.*.log")}));

	QFileInfoList list = tmpPath.entryInfoList();
	list.removeAll(mLogFile);

	return list;
}


void LogHandler::removeOtherLogfiles()
{
	const auto otherLogFiles = getOtherLogfiles();
	for (const auto& entry : otherLogFiles)
	{
		qDebug() << "Remove old log file:" << entry.absoluteFilePath() << "|" << QFile::remove(entry.absoluteFilePath());
	}
}


void LogHandler::messageHandler(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg)
{
	getInstance().handleMessage(pType, pContext, pMsg);
}
