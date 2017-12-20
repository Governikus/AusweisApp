/*
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "controller/AppController.h"
#include "CommandLineParser.h"
#include "global/BuildHelper.h"
#include "global/DeviceInfo.h"
#include "global/LogHandler.h"
#include "SignalHandler.h"

#include <QLoggingCategory>
#include <QSslSocket>
#include <QSysInfo>
#include <QtPlugin>
#include <QThread>

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || defined(Q_OS_WINRT)
#include <QGuiApplication>
#define QAPP QGuiApplication
#else
#include <QApplication>
#define QAPP QApplication
#endif


// Includes for version API
#include <openssl/crypto.h>

#if !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(RemoteReaderManagerPlugIn)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(PcscReaderManagerPlugIn)
Q_IMPORT_PLUGIN(WebserviceActivationHandler)
#endif

Q_IMPORT_PLUGIN(InternalActivationHandler)

#if defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)
Q_IMPORT_PLUGIN(IntentActivationHandler)
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
Q_IMPORT_PLUGIN(QtQuickExtrasStylesPlugin)
Q_IMPORT_PLUGIN(QtQuickControls1Plugin)
//Q_IMPORT_PLUGIN(QtQuickControls2MaterialStylePlugin)
//Q_IMPORT_PLUGIN(QtQuickControls2UniversalStylePlugin)
Q_IMPORT_PLUGIN(QtQuickControls2Plugin)
//Q_IMPORT_PLUGIN(QtQuick2DialogsPrivatePlugin)
Q_IMPORT_PLUGIN(QtQuick2DialogsPlugin)
//Q_IMPORT_PLUGIN(QtQuickExtrasPlugin)
Q_IMPORT_PLUGIN(QtQuickLayoutsPlugin)
//Q_IMPORT_PLUGIN(QQmlLocalStoragePlugin)
//Q_IMPORT_PLUGIN(QtQuick2ParticlesPlugin)
Q_IMPORT_PLUGIN(QtQuickTemplates2Plugin)
Q_IMPORT_PLUGIN(QtQuick2WindowPlugin)
Q_IMPORT_PLUGIN(QtQuick2Plugin)
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG))
Q_IMPORT_PLUGIN(BluetoothReaderManagerPlugIn)
#endif


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(UIPlugInCli)
Q_IMPORT_PLUGIN(UIPlugInWidgets)
#endif

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS) || !defined(QT_NO_DEBUG)
Q_IMPORT_PLUGIN(UIPlugInJsonApi)
Q_IMPORT_PLUGIN(UIPlugInAidl)
Q_IMPORT_PLUGIN(UIPlugInQml)
#endif

#ifndef QT_NO_DEBUG
Q_IMPORT_PLUGIN(UIPlugInWebSocket)
#endif


using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(init)


static inline void printInfo()
{
	qCDebug(init) << "Logging to" << LogHandler::getInstance();

	qCInfo(init) << "##################################################";
	qCInfo(init) << "### ApplicationName:" << QCoreApplication::applicationName();
	qCInfo(init) << "### ApplicationVersion:" << QCoreApplication::applicationVersion();
	qCInfo(init) << "### OrganizationName:" << QCoreApplication::organizationName();
	qCInfo(init) << "### OrganizationDomain:" << QCoreApplication::organizationDomain();
	qCInfo(init) << "### Build:" << BuildHelper::getDateTime();
	qCInfo(init) << "### System:" << QSysInfo::prettyProductName();
	qCInfo(init) << "### Kernel:" << QSysInfo::kernelVersion();
	qCInfo(init) << "### Architecture:" << QSysInfo::currentCpuArchitecture();
#ifdef Q_OS_ANDROID
	qCInfo(init) << "### Device:" << DeviceInfo::getPrettyInfo();
	qCInfo(init) << "### VersionCode:" << BuildHelper::getVersionCode();
#else
	qCInfo(init) << "### Devicename:" << DeviceInfo::getName();
#endif
	qCInfo(init) << "### Qt Version:" << qVersion();
	qCInfo(init) << "### OpenSSL Version:" << QSslSocket::sslLibraryVersionString();
	qCInfo(init) << "##################################################";

	if (QSslSocket::sslLibraryVersionString() != QLatin1String(SSLeay_version(0)))
	{
		qCWarning(init) << "Linked OpenSSL Version differs:" << SSLeay_version(0);
	}

	const auto libPathes = QCoreApplication::libraryPaths();
	for (const auto& path : libPathes)
	{
		qCDebug(init) << "Library path:" << path;
	}
}


#include "config.h" // use in main only!
Q_DECL_EXPORT int main(int argc, char** argv)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
	QCoreApplication::setAttribute(Qt::AA_DisableWindowContextHelpButton);
#endif

	QCoreApplication::setOrganizationName(QStringLiteral(VENDOR));
	QCoreApplication::setOrganizationDomain(QStringLiteral(VENDOR_DOMAIN));
	QCoreApplication::setApplicationName(QStringLiteral(PRODUCT));
	QCoreApplication::setApplicationVersion(QStringLiteral(VERSION));
	QGuiApplication::setQuitOnLastWindowClosed(false);

	QAPP app(argc, argv);
	QThread::currentThread()->setObjectName(QStringLiteral("MainThread"));

	LogHandler::getInstance().init();
	SignalHandler::getInstance().init();
	CommandLineParser::getInstance().parse();
	printInfo();

	AppController controller;
	if (!controller.start())
	{
		qCCritical(init) << "Cannot start application controller, exit application";
		return EXIT_FAILURE;
	}

	SignalHandler::getInstance().setController(controller);
	return SignalHandler::getInstance().shouldQuit() ? EXIT_SUCCESS : app.exec();
}
