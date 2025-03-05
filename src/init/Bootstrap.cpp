/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#include "Bootstrap.h"

#include "BuildHelper.h"
#include "CommandLineParser.h"
#include "Env.h"
#include "LogHandler.h"
#include "SignalHandler.h"
#include "controller/AppController.h"

#include <openssl/crypto.h>

#include <QLoggingCategory>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>
#include <QScopedPointer>
#include <QSslSocket>
#include <QThread>

#include "config.h"

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(init)


#if defined(INTEGRATED_SDK)
	#ifdef Q_OS_ANDROID
		#include <QtCore/private/qandroidextras_p.h>
using QAPP = QAndroidService;

	#else
		#include <QCoreApplication>
using QAPP = QCoreApplication;
	#endif

#elif defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WINRT)
	#ifdef Q_OS_ANDROID
		#include <QtCore/private/qandroidextras_p.h>
	#endif

	#include <QGuiApplication>
using QAPP = QGuiApplication;
#else
	#include <QApplication>
using QAPP = QApplication;
#endif


static QMutex cMutex; // clazy:exclude=non-pod-global-static


static inline void printInfo()
{
	qCDebug(init) << "Logging to" << *Env::getSingleton<LogHandler>();

	qCInfo(init) << "##################################################";
	const auto& info = BuildHelper::getInformationHeader();
	for (const auto& [key, value] : info)
	{
		qCInfo(init).noquote().nospace() << "### " << key << ": " << value;
	}
	qCInfo(init) << "##################################################";

	if (QSslSocket::sslLibraryVersionString() != QLatin1String(OpenSSL_version(OPENSSL_VERSION)))
	{
		qCWarning(init) << "Linked OpenSSL Version differs:" << OpenSSL_version(OPENSSL_VERSION);
	}

	const auto libPathes = QCoreApplication::libraryPaths();
	for (const auto& path : libPathes)
	{
		qCDebug(init) << "Library path:" << path;
	}

	qCDebug(init) << "TLS backends:" << QSslSocket::availableBackends() << "| using:" << QSslSocket::activeBackend();
}


static inline QCoreApplication* initQt(int& argc, char** argv)
{
	QCoreApplication::setOrganizationName(QStringLiteral(VENDOR));
	QCoreApplication::setOrganizationDomain(QStringLiteral(VENDOR_DOMAIN));
	QCoreApplication::setApplicationName(QStringLiteral(PRODUCT));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));

#ifndef INTEGRATED_SDK
	QGuiApplication::setQuitOnLastWindowClosed(false);
#endif

#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && !defined(INTEGRATED_SDK)
	QGuiApplication::setDesktopSettingsAware(false);
#endif

#if defined(Q_OS_ANDROID) && !defined(INTEGRATED_SDK)
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	bool isServiceContext = !QNativeInterface::QAndroidApplication::isActivityContext();
	if (context.isValid() && isServiceContext)
	{
		return new QAndroidService(argc, argv);
	}
#endif

	return new QAPP(argc, argv);
}


static inline int exec(const QScopedPointer<QCoreApplication>& pApp)
{
#if defined(Q_OS_ANDROID) && !defined(INTEGRATED_SDK)
	if (QLatin1String(pApp->metaObject()->className()) == QLatin1String("QAndroidService"))
	{
		return QAndroidService::exec();
	}
#else
	Q_UNUSED(pApp)
#endif
	return QAPP::exec();
}


#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
static void restartApp(const QString& pApplicationFilePath, QStringList pArgumentList, int pArgc)
{
	if (pArgumentList.size() == pArgc)
	{
		pArgumentList.removeFirst();
	}

	const auto& show = QStringLiteral("--show");
	if (!pArgumentList.contains(show))
	{
		pArgumentList << show;
	}

	qCInfo(init) << "Attempting to start new process:" << pApplicationFilePath << ", args:" << pArgumentList;
	if (qint64 pid = -1; QProcess::startDetached(pApplicationFilePath, pArgumentList, QString(), &pid))
	{
		qCInfo(init) << "New process successfully launched, PID:" << pid;
	}
	else
	{
		qCCritical(init) << "Could not launch new process.";
	}
}


#endif


void governikus::shutdownApp()
{
	if (cMutex.try_lock())
	{
		qCDebug(init) << "Shutdown trigger omitted";
		cMutex.unlock();
		return;
	}

	qCDebug(init) << "Trigger shutdown...";
	Env::getSingleton<SignalHandler>()->quit();
	qCDebug(init) << "Wait for MainThread to be finished...";
	const QMutexLocker mutexLocker(&cMutex);
	qCDebug(init) << "MainThread finished!";
}


int governikus::initApp(int& argc, char** argv)
{
	const QScopedPointer<QCoreApplication> app(initQt(argc, argv));
	QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

	CommandLineParser::getInstance().parse();
	Env::getSingleton<LogHandler>()->init();
	Env::getSingleton<SignalHandler>()->init();
	printInfo();

	const QMutexLocker mutexLocker(&cMutex);
	AppController controller;
	Env::getSingleton<SignalHandler>()->setController([&controller]{
				QMetaObject::invokeMethod(&controller, [&controller]{
					controller.doShutdown();
				}, Qt::QueuedConnection);
			});
	controller.start();

	qCDebug(init) << "Enter main event loop...";
	const int returnCode = exec(app);

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS) || (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
	if (controller.shouldApplicationRestart())
	{
		restartApp(QCoreApplication::applicationFilePath(), QCoreApplication::arguments(), argc);
	}
#endif

	qCDebug(init) << "Leaving application... bye bye!";
	return returnCode;
}


#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_BootstrapHelper_triggerShutdown(JNIEnv*, jobject)
{
	shutdownApp();
}


}

#endif
