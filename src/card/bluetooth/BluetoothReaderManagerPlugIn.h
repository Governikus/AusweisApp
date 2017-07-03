/*!
 * BluetoothReaderManagerPlugIn.h
 *
 * \brief Implements the ReaderManagerPlugIn with Qt'S Bluetooth API.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include "CyberJackWaveDevice.h"
#include "ReaderManagerPlugIn.h"

#include <QBluetoothDeviceDiscoveryAgent>


namespace governikus
{


class BluetoothReader;
class BluetoothReaderManagerPlugInPrivate;


class BluetoothReaderManagerPlugIn
	: public ReaderManagerPlugIn
{
	Q_PLUGIN_METADATA(IID "governikus.ReaderManagerPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::ReaderManagerPlugIn)
	Q_DECLARE_PRIVATE(BluetoothReaderManagerPlugIn)
	Q_OBJECT

	private:
		BluetoothReaderManagerPlugInPrivate* const d_ptr;

		QBluetoothDeviceDiscoveryAgent mDeviceDiscoveryAgent;
		QStringList mDiscoveredReadersInCurrentScan;
		QMap<QString, QSharedPointer<CyberJackWaveDevice> > mInitializingDevices;
		QMap<QString, BluetoothReader*> mReaders;
		bool mScanInProgress;

		void onRemoveReader(const QString& pDeviceId);

	private Q_SLOTS:
		void onDeviceDisconnected(const QBluetoothDeviceInfo& pInfo);
		void onDeviceDiscovered(const QBluetoothDeviceInfo& pInfo);
		void onDeviceDiscoveryFinished();
		void onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error pError);
		void onDeviceDiscoveryCanceled();
		void onDeviceInitialized(const QBluetoothDeviceInfo& pInfo);
		void setBluetoothStatus(bool pEnabled);

	public:
		BluetoothReaderManagerPlugIn();
		void init() override;
		QList<Reader*> getReader() const override;

	public Q_SLOTS:
		void startScan() override;
		void stopScan() override;
};

} /* namespace governikus */
