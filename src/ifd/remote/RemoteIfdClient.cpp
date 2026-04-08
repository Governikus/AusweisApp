/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdClient.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "ReaderManager.h"
#include "VolatileSettings.h"
#include "messages/Discovery.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


INIT_FUNCTION([] {
			ReaderManager::addMainThreadInit([] {
				// Force construction of RemoteIfdClient in the MainThread
				Env::getSingleton<RemoteIfdClient>();
			});
		})


#ifdef Q_OS_ANDROID
extern "C"
{

JNIEXPORT void JNICALL Java_com_governikus_ausweisapp2_AusweisApp2LocalIfdService_notifyLocalIfdDisabled(JNIEnv* pEnv, jobject pObj)
{
	Q_UNUSED(pObj)
	Q_UNUSED(pEnv)

	QMetaObject::invokeMethod(QCoreApplication::instance(), [] {
				qDebug() << "Local IFD detection was stopped. Resetting DatagramHandler.";
				Env::getSingleton<RemoteIfdClient>()->resetDatagramHandler();
			}, Qt::BlockingQueuedConnection);
}


}
#endif


RemoteIfdClient::RemoteIfdClient()
	: IfdClientImpl()
	, mDatagramHandler()
	, mIfdList(Env::create<IfdList*>())
{
	connect(mIfdList.data(), &IfdList::fireDeviceAppeared, this, &IfdClient::fireDeviceAppeared);
	connect(mIfdList.data(), &IfdList::fireDeviceUpdated, this, &IfdClient::fireDeviceUpdated);
	connect(mIfdList.data(), &IfdList::fireDeviceVanished, this, &IfdClient::fireDeviceVanished);

}


void RemoteIfdClient::resetDatagramHandler()
{
	mDatagramHandler.reset(Env::create<DatagramHandler*>());
	connect(mDatagramHandler.data(), &DatagramHandler::fireNewMessage, this, &RemoteIfdClient::onNewMessage);
}


bool RemoteIfdClient::enableLocalIfdDetection(bool pEnabled) const
{
	const auto& serviceToken = Env::getSingleton<AppSettings>()->getGeneralSettings().getIfdServiceToken();
	if (serviceToken.isEmpty())
	{
		qDebug() << "Local IFD workflow is not running";
		return false;
	}

#ifdef Q_OS_ANDROID
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if (!context.isValid())
	{
		qWarning() << "Invalid Android context";
		return false;
	}

	qDebug() << "Change Local IFD detection. Enabled:" << pEnabled;
	const auto& jServiceToken = QJniObject::fromString(serviceToken);
	QJniObject::callStaticMethod<void>(
			"com/governikus/ausweisapp2/AusweisApp2LocalIfdService",
			"enableLocalIfdDetection",
			"(Landroid/content/Context;Ljava/lang/String;Z)V",
			context.object<jobject>(),
			jServiceToken.object<jstring>(),
			pEnabled);
#else
	Q_UNUSED(pEnabled)
#endif

	return true;

}


void RemoteIfdClient::onNewMessage(const QByteArray& pData, const QHostAddress& pAddress)
{
	QJsonObject obj;
	{
		obj = IfdMessage::parseByteArray(pData);
	}

	Discovery discovery(obj);
	if (discovery.isIncomplete())
	{
		qCDebug(ifd) << "Discarding unparsable message";
		return;
	}

	// addresses was introduced with 2.3.2. Use source for older versions.
	if (discovery.addressesMissing())
	{
		discovery.setAddresses({pAddress});
		if (discovery.addressesMissing())
		{
			qCCritical(ifd) << "Dropping message from unparsable address:" << pAddress;
			return;
		}
	}

	mIfdList->update(discovery);
}


void RemoteIfdClient::startDetection()
{
	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		if (!mDatagramHandler.isNull())
		{
			qCDebug(ifd) << "Detection already started";
			return;
		}

		if (enableLocalIfdDetection(false))
		{
			return;
		}
	}

	resetDatagramHandler();
}


void RemoteIfdClient::stopDetection()
{
	if (mDatagramHandler.isNull())
	{
		qCDebug(ifd) << "Detection already stopped";
		return;
	}

	mDatagramHandler.reset();
	mIfdList->clear();

	if (!Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		Q_UNUSED(enableLocalIfdDetection(true))
	}
}


bool RemoteIfdClient::isDetecting()
{
	return !mDatagramHandler.isNull();
}


QList<QSharedPointer<IfdListEntry>> RemoteIfdClient::getAnnouncingRemoteDevices() const
{
	return mIfdList->getIfdList();
}


void RemoteIfdClient::requestRemoteDevices()
{
	Q_EMIT fireRemoteDevicesInfo(mIfdList->getIfdList());
}


QList<RemoteServiceSettings::RemoteInfo> RemoteIfdClient::getConnectedDeviceInfos()
{
	const RemoteServiceSettings& settings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	auto remoteInfos = settings.getRemoteInfos();
	QList<RemoteServiceSettings::RemoteInfo> result;
	for (const auto& info : std::as_const(remoteInfos))
	{
		const auto& deviceIDs = getConnectedDeviceIDs();
		for (const auto& id : deviceIDs)
		{
			if (info.getFingerprint() == id)
			{
				result.append(info);
			}
		}
	}
	return result;
}
