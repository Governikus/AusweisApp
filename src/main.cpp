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

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(WebserviceActivationHandler)
#endif

#endif


#if defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(IntentActivationHandler)
#endif

#endif


#if defined(Q_OS_IOS)
Q_IMPORT_PLUGIN(IosReaderManagerPlugIn)

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(CustomSchemeActivationHandler)

Q_IMPORT_PLUGIN(QJpegPlugin)
Q_IMPORT_PLUGIN(QSvgPlugin)
#endif

#endif


#if defined(Q_OS_ANDROID) || !defined(QT_NO_DEBUG)
Q_IMPORT_PLUGIN(UIPlugInAidl)
#endif

#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(UIPlugInQml)
#endif

#if defined(INTEGRATED_SDK) && !defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(UIPlugInFunctional)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(UIPlugInWebSocket)
#endif

#if !defined(Q_OS_IOS) || defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(UIPlugInJson)
#endif

Q_DECL_EXPORT int main(int argc, char** argv)
{
	return governikus::initApp(argc, argv);
}
