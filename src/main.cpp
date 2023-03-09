/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "Bootstrap.h"

#include "init/config.h"

#include <QtPlugin>


Q_IMPORT_PLUGIN(SimulatorReaderManagerPlugIn)

#if !defined(Q_OS_WINRT) && !defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(RemoteIfdReaderManagerPlugIn)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && !defined(Q_OS_WINRT)
Q_IMPORT_PLUGIN(PcscReaderManagerPlugIn)

	#if !defined(INTEGRATED_SDK) || defined(CONTAINER_SDK)
Q_IMPORT_PLUGIN(UIPlugInWebService)
Q_IMPORT_PLUGIN(UIPlugInProxy)
	#endif

	#if defined(USE_SMARTEID) && (defined(CONTAINER_SDK) || !defined(QT_NO_DEBUG))
Q_IMPORT_PLUGIN(SmartReaderManagerPlugIn)
	#endif

#endif


#if defined(Q_OS_ANDROID)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)

	#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(UIPlugInScheme)
Q_IMPORT_PLUGIN(UIPlugInLocalIfd)

		#ifdef USE_SMARTEID
Q_IMPORT_PLUGIN(SmartReaderManagerPlugIn)
		#endif

	#else
Q_IMPORT_PLUGIN(LocalIfdReaderManagerPlugIn)
	#endif

#endif


#if defined(Q_OS_IOS)
Q_IMPORT_PLUGIN(NfcReaderManagerPlugIn)

	#ifndef INTEGRATED_SDK
Q_IMPORT_PLUGIN(UIPlugInScheme)

		#ifdef USE_SMARTEID
Q_IMPORT_PLUGIN(SmartReaderManagerPlugIn)
		#endif

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

#if defined(INTEGRATED_SDK) && !defined(Q_OS_ANDROID) && !defined(CONTAINER_SDK)
Q_IMPORT_PLUGIN(UIPlugInFunctional)
#endif

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS) && (!defined(INTEGRATED_SDK) || defined(CONTAINER_SDK))
Q_IMPORT_PLUGIN(UIPlugInAutomatic)
Q_IMPORT_PLUGIN(UIPlugInWebSocket)
#endif

#if !defined(Q_OS_IOS) || defined(INTEGRATED_SDK)
Q_IMPORT_PLUGIN(UIPlugInJson)
#endif

Q_DECL_EXPORT int main(int argc, char** argv)
{
	return governikus::initApp(argc, argv);
}
