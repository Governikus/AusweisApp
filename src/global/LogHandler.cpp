/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "LogHandler.h"

#include "BreakPropertyBindingDiagnosticLogFilter.h"
#include "SingletonHelper.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 0))
#include <QScopeGuard>
#else
#include "ScopeGuard.h"
#endif

#include <QCoreApplication>
#include <QDir>
#include <QStringBuilder>

using namespace governikus;

defineSingleton(LogHandler)

Q_DECLARE_LOGGING_CATEGORY(fileprovider)
Q_DECLARE_LOGGING_CATEGORY(securestorage)
Q_DECLARE_LOGGING_CATEGORY(configuration)

#define LOGCAT(name) QString::fromLatin1(name().categoryName())


#if !defined(Q_OS_ANDROID) && !defined(QT_USE_JOURNALD)
#define ENABLE_MESSAGE_PATTERN
#endif


LogHandler::LogHandler()
	: QObject()
	, mEnvPattern(!qEnvironmentVariableIsEmpty("QT_MESSAGE_PATTERN"))
	, mFunctionFilenameSize(74)
	, mBacklogPosition(0)
	, mCriticalLog(false)
	, mCriticalLogWindow(10)
	, mCriticalLogIgnore({LOGCAT(fileprovider), LOGCAT(securestorage), LOGCAT(configuration)})
	, mMessagePattern(QStringLiteral("%{category} %{time yyyy.MM.dd hh:mm:ss.zzz} %{threadid} %{if-debug} %{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{function}(%{file}:%{line}) %{message}"))
	, mDefaultMessagePattern(QStringLiteral("%{if-category}%{category}: %{endif}%{message}")) // as defined in qlogging.cpp
	, mLogFile(getLogFileTemplate())
	, mHandler(nullptr)
	, mUseHandler(true)
	, mFilePrefix("/src/")
	, mMutex()
{
#ifndef QT_NO_DEBUG
	new BreakPropertyBindingDiagnosticLogFilter(this);
#endif
}


LogHandler::~LogHandler()
{
	reset();
}


LogHandler& LogHandler::getInstance()
{
	return *Instance;
}


QString LogHandler::getLogFileTemplate()
{
	// if you change value you need to adjust getOtherLogfiles()
	return QDir::tempPath() % QLatin1Char('/') % QCoreApplication::applicationName() % QStringLiteral(".XXXXXX.log");
}


void LogHandler::reset()
{
	const QMutexLocker mutexLocker(&mMutex);
	if (isInitialized())
	{
		qInstallMessageHandler(nullptr);
		mHandler = nullptr;
	}
}


void LogHandler::init()
{
	const QMutexLocker mutexLocker(&mMutex);
	if (!isInitialized())
	{
		if (useLogfile())
		{
			mLogFile.open();
		}
		mHandler = qInstallMessageHandler(&LogHandler::messageHandler);

		// Avoid deadlock with subsequent logging of this call.
		QMetaObject::invokeMethod(this, &LogHandler::removeOldLogfiles, Qt::QueuedConnection);
	}
}


bool LogHandler::isInitialized() const
{
	return mHandler;
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


QByteArray LogHandler::readLogFile(qint64 pStart, qint64 pLength)
{
	if (mLogFile.isOpen() && mLogFile.isReadable())
	{
		const auto currentPos = mLogFile.pos();
		const auto resetPosition = qScopeGuard([this, currentPos] {
					mLogFile.seek(currentPos);
				});

		mLogFile.seek(pStart);
		return pLength > 0 ? mLogFile.read(pLength) : mLogFile.readAll();
	}

	if (useLogfile())
	{
		//: LABEL ALL_PLATFORMS
		return tr("An error occurred in log handling: %1").arg(mLogFile.errorString()).toUtf8();
	}

	return QByteArray();
}


QByteArray LogHandler::getBacklog(bool pAll)
{
	const QMutexLocker mutexLocker(&mMutex);
	return readLogFile(pAll ? 0 : mBacklogPosition);
}


QByteArray LogHandler::getCriticalLogWindow()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (mCriticalLog)
	{
		const auto first = qAsConst(mCriticalLogWindow).first();
		const auto last = qAsConst(mCriticalLogWindow).last();
		return readLogFile(first.mPosition, last.mPosition - first.mPosition + last.mLength);
	}

	return QByteArray();
}


bool LogHandler::hasCriticalLog() const
{
	return mCriticalLog;
}


int LogHandler::getCriticalLogCapacity() const
{
	return mCriticalLogWindow.capacity();
}


void LogHandler::setCriticalLogCapacity(int pSize)
{
	const QMutexLocker mutexLocker(&mMutex);
	mCriticalLogWindow.setCapacity(pSize);
}


QDateTime LogHandler::getFileDate(const QFileInfo& pInfo)
{
	const auto& dateTime = pInfo.birthTime();
	if (dateTime.isValid())
	{
		return dateTime;
	}

	return pInfo.metadataChangeTime();
}


QDateTime LogHandler::getCurrentLogfileDate() const
{
	return getFileDate(mLogFile);
}


void LogHandler::resetBacklog()
{
	const QMutexLocker mutexLocker(&mMutex);
	mBacklogPosition = mLogFile.pos();
	mCriticalLog = false;
	mCriticalLogWindow.clear();
}


void LogHandler::copyMessageLogContext(const QMessageLogContext& pSource, QMessageLogContext& pDestination, const QByteArray& pFilename, const QByteArray& pFunction, const QByteArray& pCategory)
{
	pDestination.file = pFilename.isNull() ? pSource.file : pFilename.constData();
	pDestination.function = pFunction.isNull() ? pSource.function : pFunction.constData();
	pDestination.category = pCategory.isNull() ? pSource.category : pCategory.constData();

	pDestination.line = pSource.line;
	pDestination.version = pSource.version;
}


QByteArray LogHandler::formatFilename(const char* const pFilename) const
{
	QByteArray filename(pFilename);

	// Normalize the file name
	filename.replace(QByteArrayLiteral("\\"), "/");

	// Remove useless path
	return filename.mid(filename.lastIndexOf(mFilePrefix) + mFilePrefix.size());
}


QByteArray LogHandler::formatFunction(const char* const pFunction, const QByteArray& pFilename, int pLine) const
{
	QByteArray function(pFunction);

	// Remove namespace governikus::
	function.replace(QByteArrayLiteral("governikus::"), "");

	// Remove the parameter list
	const auto start = function.indexOf('(');
	const auto end = function.indexOf(')');
	function = function.left(start) + function.mid(end + 1);

	if (function.endsWith(" const"))
	{
		function = function.left(function.size() - 6);
	}

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


QByteArray LogHandler::formatCategory(const QByteArray& pCategory) const
{
	const int MAX_CATEGORY_LENGTH = 10;
	if (pCategory.length() > MAX_CATEGORY_LENGTH)
	{
		return pCategory.left(MAX_CATEGORY_LENGTH - 3) + QByteArrayLiteral("...");
	}

	return pCategory + QByteArray(MAX_CATEGORY_LENGTH - pCategory.size(), ' ');
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

	const QByteArray& filename = formatFilename(pContext.file);
	const QByteArray& function = formatFunction(pContext.function, filename, pContext.line);
	const QByteArray& category = formatCategory(pContext.category);

	QMessageLogContext ctx;
	copyMessageLogContext(pContext, ctx, filename, function, category);

	const QString& message = mEnvPattern ? pMsg : getPaddedLogMsg(ctx, pMsg);

	qSetMessagePattern(mMessagePattern);

#ifdef Q_OS_WIN
	const QLatin1String lineBreak("\r\n");
#else
	const QLatin1Char lineBreak('\n');
#endif

	const QString logMsg = qFormatLogMessage(pType, ctx, message) + lineBreak;
	handleLogWindow(pType, pContext.category, logMsg);
	logToFile(logMsg);

	if (Q_LIKELY(mUseHandler))
	{
#ifdef ENABLE_MESSAGE_PATTERN
		mHandler(pType, ctx, message);
#else
		qSetMessagePattern(mDefaultMessagePattern);
		mHandler(pType, ctx, pMsg);
#endif
	}

	Q_EMIT fireRawLog(pMsg, QString::fromLatin1(pContext.category));
	Q_EMIT fireLog(logMsg);
}


void LogHandler::handleLogWindow(QtMsgType pType, const char* pCategory, const QString& pMsg)
{
	if (!useLogfile())
	{
		return;
	}

	if (mCriticalLog && mCriticalLogWindow.isFull())
	{
		return;
	}
	else if (pType == QtCriticalMsg && !mCriticalLogIgnore.contains(QLatin1String(pCategory)))
	{
		mCriticalLog = true;
	}

	mCriticalLogWindow.append({mLogFile.pos(), pMsg.size()});
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
	tmpPath.setSorting(QDir::Time);
	tmpPath.setFilter(QDir::Files);
	tmpPath.setNameFilters(QStringList({QCoreApplication::applicationName() + QStringLiteral(".*.log")}));

	QFileInfoList list = tmpPath.entryInfoList();
	list.removeAll(mLogFile);

	return list;
}


void LogHandler::removeOldLogfiles()
{
	const auto& threshold = QDateTime::currentDateTime().addDays(-14);
	const QFileInfoList& logfileInfos = getOtherLogfiles();
	for (const QFileInfo& entry : logfileInfos)
	{
		if (entry.fileTime(QFileDevice::FileModificationTime) < threshold)
		{
			const auto result = QFile::remove(entry.absoluteFilePath());
			qDebug() << "Auto-remove old logfile:" << entry.absoluteFilePath() << '|' << result;
		}
	}
}


bool LogHandler::removeOtherLogfiles()
{
	const auto otherLogFiles = getOtherLogfiles();
	for (const auto& entry : otherLogFiles)
	{
		const auto result = QFile::remove(entry.absoluteFilePath());
		qDebug() << "Remove old logfile:" << entry.absoluteFilePath() << '|' << result;
	}

	return !otherLogFiles.isEmpty();
}


void LogHandler::setLogfile(bool pEnable)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (pEnable)
	{
		if (!mLogFile.isOpen())
		{
			mLogFile.setFileTemplate(getLogFileTemplate());
			mLogFile.open();
		}
	}
	else
	{
		if (mLogFile.isOpen())
		{
			mLogFile.close();
			mLogFile.remove();
			mBacklogPosition = 0;
			mCriticalLog = false;
			mCriticalLogWindow.clear();
		}
		mLogFile.setFileTemplate(QString());
	}
}


bool LogHandler::useLogfile() const
{
	return !mLogFile.fileTemplate().isNull();
}


void LogHandler::setUseHandler(bool pEnable)
{
	mUseHandler = pEnable;
}


bool LogHandler::useHandler() const
{
	return mUseHandler;
}


void LogHandler::messageHandler(QtMsgType pType, const QMessageLogContext& pContext, const QString& pMsg)
{
	getInstance().handleMessage(pType, pContext, pMsg);
}
