/*
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AppController.h"
#include "CommandLineParser.h"
#include "global/BuildHelper.h"
#include "global/LogHandler.h"
#include "SignalHandler.h"

#include <openssl/crypto.h> // version API

#include <QLoggingCategory>
#include <QScopedPointer>
#include <QSslSocket>
#include <QtPlugin>
#include <QThread>

#include "config.h" // use in main only!

#ifdef ANDROID_BUILD_AAR
	#include <QAndroidService>
	#define QAPP QAndroidService

#elif defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WINRT)
	#ifdef Q_OS_ANDROID
		#include <QAndroidService>
		#include <QtAndroid>
	#endif

	#include <QGuiApplication>
	#define QAPP QGuiApplication
#else
	#include <QApplication>
	#define QAPP QApplication
#endif


#if !defined(Q_OS_WINRT) && !defined(ANDROID_BUILD_AAR)
Q_IMPORT_PLUGIN(RemoteReaderManagerPlugIn)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(PcscReaderManagerPlugIn)
Q_IMPORT_PLUGIN(WebserviceActivationHandler)
#endif

Q_IMPORT_PLUGIN(InternalActivationHandler)

#if defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)

#ifndef ANDROID_BUILD_AAR
Q_IMPORT_PLUGIN(IntentActivationHandler)
#endif

#endif


#if defined(Q_OS_IOS)
Q_IMPORT_PLUGIN(CustomSchemeActivationHandler)

Q_IMPORT_PLUGIN(QJpegPlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
Q_IMPORT_PLUGIN(QtQmlModelsPlugin)
Q_IMPORT_PLUGIN(QtQmlStateMachinePlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPlugin)
Q_IMPORT_PLUGIN(QtGraphicalEffectsPrivatePlugin)

// Do not delete the comments to avoid searching for the class name
//Q_IMPORT_PLUGIN(QtQuickControls2MaterialStylePlugin)
//Q_IMPORT_PLUGIN(QtQuickControls2UniversalStylePlugin)
Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
//Q_IMPORT_PLUGIN(QQmlLocalStoragePlugin)
//Q_IMPORT_PLUGIN(QtQuick2ParticlesPlugin)
Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
#endif

#if (defined(Q_OS_ANDROID) && !defined(ANDROID_BUILD_AAR)) || defined(Q_OS_IOS) || (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG) && !defined(ANDROID_BUILD_AAR))
Q_IMPORT_PLUGIN(BluetoothReaderManagerPlugIn)
#endif


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(UIPlugInCli)
Q_IMPORT_PLUGIN(UIPlugInWidgets)
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || !defined(QT_NO_DEBUG)
Q_IMPORT_PLUGIN(UIPlugInAidl)
#endif

#if (defined(Q_OS_ANDROID) && !defined(ANDROID_BUILD_AAR)) || defined(Q_OS_IOS) || (!defined(QT_NO_DEBUG) && !defined(ANDROID_BUILD_AAR))
Q_IMPORT_PLUGIN(UIPlugInQml)
#endif

Q_IMPORT_PLUGIN(UIPlugInJsonApi)
Q_IMPORT_PLUGIN(UIPlugInWebSocket)


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(init)


static inline void printInfo()
{
	qCDebug(init) << "Logging to" << *Env::getSingleton<LogHandler>();

	qCInfo(init) << "##################################################";
	const auto& info = BuildHelper::getInformationHeader();
	for (const auto& entry : info)
	{
		qCInfo(init).noquote().nospace() << "### " << entry.first << ": " << entry.second;
	}
	qCInfo(init) << "##################################################";

	#if OPENSSL_VERSION_NUMBER < 0x10100000L
		#define OpenSSL_version SSLeay_version
		#define OPENSSL_VERSION SSLEAY_VERSION
	#endif

	if (QSslSocket::sslLibraryVersionString() != QLatin1String(OpenSSL_version(OPENSSL_VERSION)))
	{
		qCWarning(init) << "Linked OpenSSL Version differs:" << OpenSSL_version(OPENSSL_VERSION);
	}

	const auto libPathes = QCoreApplication::libraryPaths();
	for (const auto& path : libPathes)
	{
		qCDebug(init) << "Library path:" << path;
	}
}


static inline QCoreApplication* initQt(int& argc, char** argv)
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

	QCoreApplication::setOrganizationName(QStringLiteral(VENDOR));
	QCoreApplication::setOrganizationDomain(QStringLiteral(VENDOR_DOMAIN));
	QCoreApplication::setApplicationName(QStringLiteral(PRODUCT));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));

#ifndef ANDROID_BUILD_AAR
	QGuiApplication::setQuitOnLastWindowClosed(false);
#endif

#if defined(Q_OS_ANDROID) && !defined(ANDROID_BUILD_AAR)
	if (QtAndroid::androidService().isValid())
	{
		return new QAndroidService(argc, argv);
	}
#endif

	return new QAPP(argc, argv);
}


Q_DECL_EXPORT int main(int argc, char** argv)
{
	const QScopedPointer<QCoreApplication> app(initQt(argc, argv));
	QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

	CommandLineParser::getInstance().parse();
	Env::getSingleton<LogHandler>()->init();
	SignalHandler::getInstance().init();
	printInfo();

	AppController controller;
	if (!controller.start())
	{
		qCCritical(init) << "Cannot start application controller, exit application";
		return EXIT_FAILURE;
	}

	SignalHandler::getInstance().setController(controller);
	return SignalHandler::getInstance().shouldQuit() ? EXIT_SUCCESS : app->exec();
}
