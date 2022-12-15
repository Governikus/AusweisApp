/*
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#include "LogHandler.h"

#include "SingletonHelper.h"

#include <QCoreApplication>
#include <QDir>
#include <QScopeGuard>
#include <QStringBuilder>

#ifdef Q_OS_ANDROID
	#include <QJniObject>
#endif

using namespace governikus;

defineSingleton(LogHandler)

Q_DECLARE_LOGGING_CATEGORY(fileprovider)
Q_DECLARE_LOGGING_CATEGORY(securestorage)
Q_DECLARE_LOGGING_CATEGORY(configuration)

#define LOGCAT(name) QString::fromLatin1(name().categoryName())


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(QT_USE_JOURNALD)
	#define ENABLE_MESSAGE_PATTERN
#endif


LogHandler::LogHandler()
	: mEventHandler()
	, mEnvPattern(!qEnvironmentVariableIsEmpty("QT_MESSAGE_PATTERN"))
	, mFunctionFilenameSize(74)
	, mBacklogPosition(0)
	, mCriticalLog(false)
	, mCriticalLogWindow(10)
	, mCriticalLogIgnore({LOGCAT(fileprovider), LOGCAT(securestorage), LOGCAT(configuration)})
	, mMessagePattern(QStringLiteral("%{category} %{time yyyy.MM.dd hh:mm:ss.zzz} %{threadid} %{if-debug} %{endif}%{if-info}I%{endif}%{if-warning}W%{endif}%{if-critical}C%{endif}%{if-fatal}F%{endif} %{function}(%{file}:%{line}) %{message}"))
	, mDefaultMessagePattern(QStringLiteral("%{if-category}%{category}: %{endif}%{message}")) // as defined in qlogging.cpp
	, mLogFile()
	, mHandler(nullptr)
	, mUseHandler(true)
	, mAutoRemove(true)
	, mUseLogFile(true)
	, mFilePrefix("/src/")
	, mMutex()
{
}


LogHandler::~LogHandler()
{
	reset();
}


QString LogHandler::getLogFileTemplate()
{
	// if you change value you need to adjust getOtherLogFiles()
	return QDir::tempPath() % QLatin1Char('/') % QCoreApplication::applicationName() % QStringLiteral(".XXXXXX.log");
}


void LogHandler::reset()
{
	const QMutexLocker mutexLocker(&mMutex);
	if (isInstalled())
	{
		qInstallMessageHandler(nullptr);
		mHandler = nullptr;
	}
}


void LogHandler::init()
{
#ifndef Q_OS_ANDROID
	const
#endif
	QMutexLocker mutexLocker(&mMutex);

#ifndef QT_NO_DEBUG
	QLoggingCategory::setFilterRules(QStringLiteral("qt.qml.binding.removal.info=true"));
#endif

	if (mEventHandler.isNull())
	{
		mEventHandler = new LogEventHandler();
		QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, mEventHandler.data(), &QObject::deleteLater);
		QObject::connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, mEventHandler.data(), [this] {
				mEventHandler.clear(); // clear immediately, otherwise logging in &QObject::destroyed is dangerous!
			});
	}

	if (mLogFile.isNull())
	{
		mLogFile = new QTemporaryFile(getLogFileTemplate());
		QObject::connect(QCoreApplication::instance(), &QCoreApplication::destroyed, mLogFile.data(), [this] {
				delete this->mLogFile.data();
			});

		setAutoRemove(mAutoRemove);
		setLogFileInternal(mUseLogFile);

		// Avoid deadlock with subsequent logging of this call.
		QMetaObject::invokeMethod(mLogFile.data(), [this] {removeOldLogFiles();}, Qt::QueuedConnection);
	}

	if (!isInstalled())
	{
		mHandler = qInstallMessageHandler(&LogHandler::messageHandler);

#ifdef Q_OS_ANDROID
		mutexLocker.unlock();
		QJniObject::callStaticMethod<void>("com/governikus/ausweisapp2/LogHandler", "init");
#endif
	}
}


const LogEventHandler* LogHandler::getEventHandler() const
{
	return mEventHandler;
}


bool LogHandler::isInstalled() const
{
	return mHandler;
}


void LogHandler::setAutoRemove(bool pRemove)
{
	mAutoRemove = pRemove;
	if (mLogFile)
	{
		mLogFile->setAutoRemove(mAutoRemove);
	}
}


void LogHandler::logToFile(const QString& pOutput)
{
	if (mLogFile && mLogFile->isOpen() && mLogFile->isWritable())
	{
		mLogFile->write(pOutput.toUtf8());
		mLogFile->flush();
	}
}


QByteArray LogHandler::readLogFile(qint64 pStart, qint64 pLength)
{
	if (mLogFile && mLogFile->isOpen() && mLogFile->isReadable())
	{
		const auto currentPos = mLogFile->pos();
		const auto resetPosition = qScopeGuard([this, currentPos] {
				mLogFile->seek(currentPos);
			});

		mLogFile->seek(pStart);
		return pLength > 0 ? mLogFile->read(pLength) : mLogFile->readAll();
	}

	if (useLogFile())
	{
		//: LABEL ALL_PLATFORMS
		return QObject::tr("An error occurred in log handling: %1").arg(mLogFile->errorString()).toUtf8();
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
		const auto first = std::as_const(mCriticalLogWindow).first();
		const auto last = std::as_const(mCriticalLogWindow).last();
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
	if (const auto& dateTime = pInfo.birthTime(); dateTime.isValid())
	{
		return dateTime;
	}

	if (const auto& dateTime = pInfo.lastModified(); dateTime.isValid())
	{
		return dateTime;
	}

	return pInfo.metadataChangeTime();
}


QDateTime LogHandler::getCurrentLogFileDate() const
{
	return useLogFile() ? getFileDate(QFileInfo(*mLogFile)) : QDateTime();
}


void LogHandler::resetBacklog()
{
	const QMutexLocker mutexLocker(&mMutex);

	if (useLogFile())
	{
		mBacklogPosition = mLogFile->pos();
		mCriticalLog = false;
		mCriticalLogWindow.clear();
	}
}


void LogHandler::copyMessageLogContext(const QMessageLogContext& pSource,
		QMessageLogContext& pDestination,
		const QByteArray& pFilename,
		const QByteArray& pFunction,
		const QByteArray& pCategory) const
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

	// Remove the parameter list
	function = function.left(function.indexOf('('));

	// Remove namespace governikus::
	function.replace(QByteArrayLiteral("governikus::"), "");

	// Remove the return type (if any)
	if (function.indexOf(' ') != -1)
	{
		function = function.mid(function.lastIndexOf(' ') + 1);
	}

	// Trim function name
	const auto size = mFunctionFilenameSize - 3 - pFilename.size() - QString::number(pLine).size();

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
	if (pCategory.length() > MAX_CATEGORY_LENGTH)
	{
		return pCategory.left(MAX_CATEGORY_LENGTH - 3) + QByteArrayLiteral("...");
	}

	return pCategory + QByteArray(MAX_CATEGORY_LENGTH - pCategory.size(), ' ');
}


QString LogHandler::getPaddedLogMsg(const QMessageLogContext& pContext, const QString& pMsg) const
{
	const auto paddingSize = (pContext.function == nullptr && pContext.file == nullptr && pContext.line == 0) ?
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
#ifdef QT_NO_DEBUG
	if (pMsg.contains(QLatin1String("stale focus object")))
	{
		return;
	}
#endif

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

	if (mEventHandler)
	{
		Q_EMIT mEventHandler->fireRawLog(pMsg, QString::fromLatin1(pContext.category));
		Q_EMIT mEventHandler->fireLog(logMsg);
	}
}


void LogHandler::handleLogWindow(QtMsgType pType, const char* pCategory, const QString& pMsg)
{
	if (!useLogFile())
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

	mCriticalLogWindow.append({mLogFile->pos(), pMsg.size()});
}


bool LogHandler::copy(const QString& pDest)
{
	const QMutexLocker mutexLocker(&mMutex);

	if (useLogFile())
	{
		return copyOther(mLogFile->fileName(), pDest);
	}

	return false;
}


bool LogHandler::copyOther(const QString& pSource, const QString& pDest) const
{
	if (pDest.trimmed().isEmpty())
	{
		return false;
	}

	if (mLogFile && pSource != mLogFile->fileName() && !getOtherLogFiles().contains(QFileInfo(pSource)))
	{
		return false;
	}

	if (QFile::exists(pDest) && !QFile::remove(pDest))
	{
		return false;
	}

	return QFile::copy(pSource, pDest);
}


QFileInfoList LogHandler::getOtherLogFiles() const
{
	QDir tmpPath = QDir::temp();
	tmpPath.setSorting(QDir::Time);
	tmpPath.setFilter(QDir::Files);
	tmpPath.setNameFilters(QStringList({QCoreApplication::applicationName() + QStringLiteral(".*.log")}));

	QFileInfoList list = tmpPath.entryInfoList();

	if (useLogFile())
	{
		list.removeAll(QFileInfo(*mLogFile));
	}

	return list;
}


void LogHandler::removeOldLogFiles()
{
	const auto& threshold = QDateTime::currentDateTime().addDays(-14);
	const QFileInfoList& logfileInfos = getOtherLogFiles();
	for (const QFileInfo& entry : logfileInfos)
	{
		if (entry.fileTime(QFileDevice::FileModificationTime) < threshold)
		{
			const auto result = QFile::remove(entry.absoluteFilePath());
			qDebug() << "Auto-remove old logfile:" << entry.absoluteFilePath() << '|' << result;
		}
	}
}


bool LogHandler::removeOtherLogFiles()
{
	const auto otherLogFiles = getOtherLogFiles();
	for (const auto& entry : otherLogFiles)
	{
		const auto result = QFile::remove(entry.absoluteFilePath());
		qDebug() << "Remove old logfile:" << entry.absoluteFilePath() << '|' << result;
	}

	return !otherLogFiles.isEmpty();
}


void LogHandler::setLogFile(bool pEnable)
{
	const QMutexLocker mutexLocker(&mMutex);
	setLogFileInternal(pEnable);
}


void LogHandler::setLogFileInternal(bool pEnable)
{
	mUseLogFile = pEnable;

	if (!mLogFile)
	{
		return;
	}

	if (mUseLogFile)
	{
		if (!mLogFile->isOpen())
		{
			mLogFile->setFileTemplate(getLogFileTemplate());
			mLogFile->open();
		}
	}
	else
	{
		if (mLogFile->isOpen())
		{
			mLogFile->close();
			mLogFile->remove();
			mBacklogPosition = 0;
			mCriticalLog = false;
			mCriticalLogWindow.clear();
		}
		mLogFile->setFileTemplate(QString());
	}
}


bool LogHandler::useLogFile() const
{
	return mLogFile && mUseLogFile && !mLogFile->fileTemplate().isNull();
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


#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT void Java_com_governikus_ausweisapp2_LogHandler_propagate(JNIEnv* pEnv, jobject pObj,
		int pLevel, jstring pCategory, jstring pFile, jstring pFunction, jstring pMsg)
{
	Q_UNUSED(pObj)

	const auto* category = pCategory ? pEnv->GetStringUTFChars(pCategory, 0) : nullptr;
	const bool useFuncSignature = qstrcmp(category, "bdr") != 0 && qstrcmp(category, "dts") != 0; // no useful information provided

	const auto* file = useFuncSignature && pFile ? pEnv->GetStringUTFChars(pFile, 0) : nullptr;
	const auto* func = useFuncSignature && pFunction ? pEnv->GetStringUTFChars(pFunction, 0) : nullptr;
	const auto* msg = pMsg ? pEnv->GetStringUTFChars(pMsg, 0) : nullptr;

	const auto& utf8Msg = QString::fromUtf8(msg);
	const QLoggingCategory loggingCategory(category);
	const auto& categoryFunction = [&loggingCategory]() -> const QLoggingCategory& {
				return loggingCategory;
			};
	const MessageLogger logger(file, 0, func, categoryFunction);
	// https://developer.android.com/reference/java/util/logging/Level
	switch (pLevel)
	{
		// SEVERE
		case 1000:
			logger.critical().noquote() << utf8Msg;
			break;

		// WARNING
		case 900:
			logger.warning().noquote() << utf8Msg;
			break;

		// INFO
		case 800:
			logger.info().noquote() << utf8Msg;
			break;

		default:
			logger.debug().noquote() << utf8Msg;
			break;
	}

	if (category)
	{
		pEnv->ReleaseStringUTFChars(pCategory, category);
	}
	if (file)
	{
		pEnv->ReleaseStringUTFChars(pFile, file);
	}
	if (func)
	{
		pEnv->ReleaseStringUTFChars(pFunction, func);
	}
	if (msg)
	{
		pEnv->ReleaseStringUTFChars(pMsg, msg);
	}
}


}
#endif

static_assert(!std::is_base_of_v<QObject, LogHandler>,
		"LogHandler cannot be a QObject because it is a Singleton that should not be destroyed at all!");


MessageLogger::MessageLogger(const char* pFile, int pLine, const char* pFunction, const std::function<const QLoggingCategory& ()>& pCategory)
	: mMessageLogger(pFile, pLine, pFunction, pCategory().categoryName())
	, mCategory(pCategory)
{
}


QDebug MessageLogger::critical() const
{
	return mMessageLogger.critical(mCategory());
}


QDebug MessageLogger::debug() const
{
	return mMessageLogger.debug(mCategory());
}


QDebug MessageLogger::info() const
{
	return mMessageLogger.info(mCategory());
}


QDebug MessageLogger::warning() const
{
	return mMessageLogger.warning(mCategory());
}
