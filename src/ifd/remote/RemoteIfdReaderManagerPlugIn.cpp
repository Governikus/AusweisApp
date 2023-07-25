/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RemoteIfdReaderManagerPlugIn.h"

#include "AppSettings.h"
#include "IfdReader.h"
#include "Reader.h"
#include "RemoteIfdClient.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_remote)

void RemoteIfdReaderManagerPlugIn::connectToPairedReaders()
{
	if (!mConnectToPairedReaders || mConnectionCheckInProgress)
	{
		return;
	}

	mConnectionCheckInProgress = true;
	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireRemoteDevicesInfo, this, &RemoteIfdReaderManagerPlugIn::continueConnectToPairedReaders);
	QMetaObject::invokeMethod(ifdClient, &IfdClient::requestRemoteDevices, Qt::QueuedConnection);
}


void RemoteIfdReaderManagerPlugIn::continueConnectToPairedReaders(const QVector<QSharedPointer<IfdListEntry>>& pRemoteDevices)
{
	const auto ifdClient = getIfdClient();

	disconnect(ifdClient, &IfdClient::fireRemoteDevicesInfo, this, &RemoteIfdReaderManagerPlugIn::continueConnectToPairedReaders);

	const RemoteServiceSettings& remoteServiceSettings = Env::getSingleton<AppSettings>()->getRemoteServiceSettings();
	for (const QSharedPointer<IfdListEntry>& remoteDevice : pRemoteDevices)
	{
		const auto& ifdDescriptor = remoteDevice->getIfdDescriptor();
		if (!ifdDescriptor.isSupported() || ifdDescriptor.isPairingAnnounced())
		{
			continue;
		}

		const QString ifdId = ifdDescriptor.getIfdId();

		// If already connected: skip.
		if (getDispatchers().contains(ifdId))
		{
			continue;
		}

		const RemoteServiceSettings::RemoteInfo remoteInfo = remoteServiceSettings.getRemoteInfo(ifdId);
		// If we find a remote info for this fingerprint (IfdId), then the remote device is paired.
		if (remoteInfo.getFingerprint() == ifdId)
		{
			QMetaObject::invokeMethod(ifdClient, [ifdClient, remoteDevice] {
					ifdClient->establishConnection(remoteDevice, QString());
				}, Qt::QueuedConnection);
		}
	}
	mConnectionCheckInProgress = false;
}


RemoteIfdReaderManagerPlugIn::RemoteIfdReaderManagerPlugIn()
	: IfdReaderManagerPlugIn(ReaderManagerPlugInType::REMOTE_IFD, true)
	, mScanTimer()
	, mConnectToPairedReaders(true)
	, mConnectionCheckInProgress(false)
{
	mScanTimer.setInterval(1000);
	connect(&mScanTimer, &QTimer::timeout, this, &RemoteIfdReaderManagerPlugIn::connectToPairedReaders);
}


RemoteIfdReaderManagerPlugIn::~RemoteIfdReaderManagerPlugIn()
{
	mScanTimer.stop();
}


void RemoteIfdReaderManagerPlugIn::startScan(bool pAutoConnect)
{
	mConnectToPairedReaders = pAutoConnect;
	const auto ifdClient = getIfdClient();
	connect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteIfdReaderManagerPlugIn::connectToPairedReaders, Qt::UniqueConnection);
	mScanTimer.start();
	IfdReaderManagerPlugIn::startScan(pAutoConnect);
}


void RemoteIfdReaderManagerPlugIn::stopScan(const QString& pError)
{
	const auto ifdClient = getIfdClient();
	disconnect(ifdClient, &IfdClient::fireDeviceAppeared, this, &RemoteIfdReaderManagerPlugIn::connectToPairedReaders);
	mScanTimer.stop();
	IfdReaderManagerPlugIn::stopScan(pError);
}


IfdClient* RemoteIfdReaderManagerPlugIn::getIfdClient()
{
	return Env::getSingleton<RemoteIfdClient>();
}
