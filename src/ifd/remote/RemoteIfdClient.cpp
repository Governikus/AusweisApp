/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdClient.h"

#include "AppSettings.h"
#include "Initializer.h"
#include "ReaderManager.h"
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

RemoteIfdClient::RemoteIfdClient()
	: IfdClientImpl()
	, mDatagramHandler()
	, mIfdList(Env::create<IfdList*>())
{
	connect(mIfdList.data(), &IfdList::fireDeviceAppeared, this, &IfdClient::fireDeviceAppeared);
	connect(mIfdList.data(), &IfdList::fireDeviceUpdated, this, &IfdClient::fireDeviceUpdated);
	connect(mIfdList.data(), &IfdList::fireDeviceVanished, this, &IfdClient::fireDeviceVanished);

}


void RemoteIfdClient::onNewMessage(const QByteArray& pData, const QHostAddress& pAddress)
{
	QJsonObject obj;
	{
		obj = IfdMessage::parseByteArray(pData);
	}

	const Discovery discovery(obj);
	if (discovery.isIncomplete())
	{
		qCDebug(ifd) << "Discarding unparsable message";
		return;
	}

	const IfdDescriptor remoteDeviceDescriptor(discovery, pAddress);
	if (remoteDeviceDescriptor.isNull())
	{
		qCCritical(ifd) << "Dropping message from unparsable address:" << pAddress;
		return;
	}

	mIfdList->update(remoteDeviceDescriptor);
}


void RemoteIfdClient::startDetection()
{
	if (!mDatagramHandler.isNull())
	{
		qCDebug(ifd) << "Detection already started";
		return;
	}

	mDatagramHandler.reset(Env::create<DatagramHandler*>());
	connect(mDatagramHandler.data(), &DatagramHandler::fireNewMessage, this, &RemoteIfdClient::onNewMessage);
	Q_EMIT fireDetectionChanged();
}


void RemoteIfdClient::stopDetection()
{
	mDatagramHandler.reset();
	mIfdList->clear();
	Q_EMIT fireDetectionChanged();
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
