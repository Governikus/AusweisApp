/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdReaderManagerPlugin.h"

#include "AppSettings.h"
#include "RemoteIfdClient.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)

void RemoteIfdReaderManagerPlugin::connectToPairedReaders() const
{
	if (!mConnectToPairedReaders)
	{
		return;
	}

	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireRemoteDevicesInfo, this, &RemoteIfdReaderManagerPlugin::continueConnectToPairedReaders);
	connect(ifdClient, &IfdClient::fireDeviceVanished, this, &RemoteIfdReaderManagerPlugin::onDeviceVanished);
	connect(ifdClient, &IfdClient::fireEstablishConnectionDone, this, &RemoteIfdReaderManagerPlugin::onEstablishConnectionDone);
	QMetaObject::invokeMethod(ifdClient, &IfdClient::requestRemoteDevices, Qt::QueuedConnection);
}


void RemoteIfdReaderManagerPlugin::continueConnectToPairedReaders(const QList<QSharedPointer<IfdListEntry>>& pRemoteDevices)
{
	const auto ifdClient = getIfdClient();

	disconnect(ifdClient, &IfdClient::fireRemoteDevicesInfo, this, &RemoteIfdReaderManagerPlugin::continueConnectToPairedReaders);

	const RemoteServiceSettings& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	for (const QSharedPointer<IfdListEntry>& remoteDevice : pRemoteDevices)
	{
		const auto& ifdDescriptor = remoteDevice->getIfdDescriptor();
		if (!ifdDescriptor.isSupported() || ifdDescriptor.isPairingAnnounced())
		{
			continue;
		}

		const QByteArray ifdId = ifdDescriptor.getIfdId();

		// If already connected: skip.
		if (getDispatchers().contains(ifdId))
		{
			mConnectionAttempts.removeAll(ifdId);
			continue;
		}

		const RemoteServiceSettings::RemoteInfo remoteInfo = remoteServiceSettings.getRemoteInfo(ifdId);
		// If we find a remote info for this fingerprint (IfdId), then the remote device is paired.
		if (remoteInfo.getFingerprint() == ifdId && !mConnectionAttempts.contains(ifdId))
		{
			mConnectionAttempts << ifdId;
			QMetaObject::invokeMethod(ifdClient, [ifdClient, remoteDevice] {
						ifdClient->establishConnection(remoteDevice, QByteArray());
					}, Qt::QueuedConnection);
		}
	}
	setInitialScanState(ReaderManagerPluginInfo::InitialScan::SUCCEEDED);
}


void RemoteIfdReaderManagerPlugin::onDeviceVanished(const QSharedPointer<IfdListEntry>& pEntry)
{
	const auto& ifdId = pEntry->getIfdDescriptor().getIfdId();
	if (mConnectionAttempts.contains(ifdId))
	{
		qCInfo(card_remote) << "Removing" << ifdId.toHex() << "from connection attempt list as it has vanished";
		mConnectionAttempts.removeAll(ifdId);
	}
}


void RemoteIfdReaderManagerPlugin::onEstablishConnectionDone(const QSharedPointer<IfdListEntry>& pEntry, const GlobalStatus& pStatus)
{
	const auto& ifdId = pEntry->getIfdDescriptor().getIfdId();
	if (mConnectionAttempts.contains(ifdId))
	{
		qCInfo(card_remote) << "Removing" << ifdId.toHex() << "from connection attempt list as the request finished with" << pStatus;
		mConnectionAttempts.removeAll(ifdId);
	}
}


RemoteIfdReaderManagerPlugin::RemoteIfdReaderManagerPlugin()
	: IfdReaderManagerPlugin(ReaderManagerPluginType::REMOTE_IFD, true)
	, mScanTimer()
	, mConnectToPairedReaders(true)
	, mConnectionAttempts()
{
	mScanTimer.setInterval(1000);
	connect(&mScanTimer, &QTimer::timeout, this, &RemoteIfdReaderManagerPlugin::connectToPairedReaders);
}


RemoteIfdReaderManagerPlugin::~RemoteIfdReaderManagerPlugin()
{
	mScanTimer.stop();
}


void RemoteIfdReaderManagerPlugin::startScan(bool pAutoConnect)
{
	mConnectToPairedReaders = pAutoConnect;
	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteIfdReaderManagerPlugin::connectToPairedReaders);
	mScanTimer.start();
	IfdReaderManagerPlugin::startScan(pAutoConnect);
}


void RemoteIfdReaderManagerPlugin::stopScan(const QString& pError)
{
	const auto ifdClient = getIfdClient();
	disconnect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteIfdReaderManagerPlugin::connectToPairedReaders);
	mScanTimer.stop();
	IfdReaderManagerPlugin::stopScan(pError);
}


IfdClient* RemoteIfdReaderManagerPlugin::getIfdClient() const
{
	return Env::getSingleton<RemoteIfdClient>();
}
