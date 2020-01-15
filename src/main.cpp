/*
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#include "Bootstrap.h"

#include "init/config.h"

#include <QtPlugin>

#if !defined(Q_OS_WINRT) && !defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(RemoteReaderManagerPlugIn)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(PcscReaderManagerPlugIn)
Q_IMPORT_PLUGIN(WebserviceActivationHandler)
#endif

#if defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(IntentActivationHandler)
#endif

#endif


#if defined(Q_OS_IOS)
Q_IMPORT_PLUGIN(QIOSIntegrationPlugin)

Q_IMPORT_PLUGIN(IosReaderManagerPlugIn)

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

#if (defined(Q_OS_ANDROID) && !defined(INTEGRATED_SDK)) || defined(Q_OS_IOS) || (defined(Q_OS_LINUX) && !defined(QT_NO_DEBUG) && !defined(INTEGRATED_SDK))
Q_IMPORT_PLUGIN(BluetoothReaderManagerPlugIn)
#endif


#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(UIPlugInWidgets)
#endif

#if defined(Q_OS_ANDROID) || !defined(QT_NO_DEBUG)
Q_IMPORT_PLUGIN(UIPlugInAidl)
#endif

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(UIPlugInQml)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(UIPlugInWebSocket)
#endif

#if !defined(Q_OS_IOS) || defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(InternalActivationHandler)
Q_IMPORT_PLUGIN(UIPlugInJson)
#endif

Q_DECL_EXPORT int main(int argc, char** argv)
{
	return governikus::initApp(argc, argv);
}
