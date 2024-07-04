/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "LocalIfdClient.h"

#include "Env.h"
#include "Initializer.h"
#include "PortFile.h"
#include "ReaderManager.h"
#include "messages/IfdVersion.h"

#include <QLoggingCategory>

#ifdef Q_OS_ANDROID
	#include "Randomizer.h"
	#include "SecureStorage.h"

	#include <QtCore/private/qandroidextras_p.h>
#endif


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;

INIT_FUNCTION([] {
			ReaderManager::addMainThreadInit([] {
				// Force construction of LocalIfdClient in the MainThread
				Env::getSingleton<LocalIfdClient>();
			});
		})

LocalIfdClient::LocalIfdClient()
	: mPsk()
	, mDevice()
#ifdef Q_OS_ANDROID
	, mServiceConnection()
#endif
{
	const Discovery discovery(QStringLiteral("LocalIfdClient"), QStringLiteral("LocalIfdClient"), PortFile::cDefaultPort, {IfdVersion::supported()});
	mDevice.reset(new IfdListEntry(IfdDescriptor(discovery, QHostAddress::LocalHostIPv6, true)));
}


LocalIfdClient::~LocalIfdClient()
{
	LocalIfdClient::stopDetection();
}


void LocalIfdClient::startDetection()
{
	if (isDetecting())
	{
		qCWarning(ifd) << "Detection was already started";
		return;
	}

#ifdef Q_OS_ANDROID
	QAndroidIntent serviceIntent(QStringLiteral("com.governikus.ausweisapp2.START_LOCAL_IFD_SERVICE"));
	QJniObject handle = serviceIntent.handle();
	if (!handle.isValid())
	{
		qCWarning(ifd) << "ServiceIntent handle is invalid";
		return;
	}

	const auto packageName = QJniObject::fromString(Env::getSingleton<SecureStorage>()->getLocalIfdPackageName());
	handle.callObjectMethod("setPackage", "(Ljava/lang/String;)Landroid/content/Intent;", packageName.object<jstring>());
	mPsk = Randomizer::getInstance().createUuid().toByteArray(QUuid::Id128);
	serviceIntent.putExtra(QStringLiteral("PSK"), mPsk);

	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if (!context.isValid())
	{
		qCWarning(ifd) << "Android Context is not valid";
		return;
	}

	mServiceConnection = QJniObject("com/governikus/ausweisapp2/AusweisApp2LocalIfdServiceConnection");

	bool isBound = context.callMethod<jboolean>("bindService", "(Landroid/content/Intent;Landroid/content/ServiceConnection;I)Z", serviceIntent.handle().object(), mServiceConnection.object(), jint(QtAndroidPrivate::BindFlag::AutoCreate)) == JNI_TRUE;
	if (!isBound)
	{
		qCWarning(ifd) << "Binding to LocalIfdService failed, is the correct AusweisApp version installed?";
		serviceDisconnected();
	}
#endif
}


void LocalIfdClient::stopDetection()
{
	if (!LocalIfdClient::isDetecting())
	{
		qCWarning(ifd) << "Detection is not running";
		return;
	}

#ifdef Q_OS_ANDROID
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if (!context.isValid())
	{
		qCWarning(ifd) << "Android Context is not valid";
		return;
	}

	context.callMethod<void>("unbindService", "(Landroid/content/ServiceConnection;)V", mServiceConnection.object());
	mServiceConnection = QJniObject();
	Q_EMIT fireDeviceVanished(mDevice);
#endif

	mPsk.clear();
}


bool LocalIfdClient::isDetecting()
{
#ifdef Q_OS_ANDROID
	return mServiceConnection.isValid();

#else
	return false;

#endif
}


const QByteArray& LocalIfdClient::getPsk() const
{
	return mPsk;
}


QList<RemoteServiceSettings::RemoteInfo> LocalIfdClient::getConnectedDeviceInfos()
{
	return {};
}


void LocalIfdClient::serviceConnected()
{
	qCInfo(ifd) << "LocalIfdService connected, connecting local client";
	Q_EMIT fireDeviceAppeared(mDevice);
}


void LocalIfdClient::serviceDisconnected()
{
	qCInfo(ifd) << "LocalIfdService disconnected";
	stopDetection();
	Q_EMIT fireDeviceVanished(mDevice);
}


#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdServiceConnection_notifyLocalIfdServiceConnected(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pObj)
	Q_UNUSED(pEnv)

	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
			Env::getSingleton<LocalIfdClient>()->serviceConnected();
		});
}


JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdServiceConnection_notifyLocalIfdServiceDisconnected(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pObj)
	Q_UNUSED(pEnv)

	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
			Env::getSingleton<LocalIfdClient>()->serviceDisconnected();
		});
}


}

#endif
