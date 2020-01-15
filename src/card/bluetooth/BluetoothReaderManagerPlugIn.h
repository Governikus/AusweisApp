/*!
 * \brief Implements the ReaderManagerPlugIn with Qt'S Bluetooth API.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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
		QMap<QString, QSharedPointer<CyberJackWaveDevice> > mInitializingDevices;
		QMap<QString, BluetoothReader*> mReaders;
		QMap<QString, int> mPendingConnections;
		QStringList mReadersDiscoveredInCurrentScan;
		int mTimerIdDiscoverPairedDevices;

		void onRemoveReader(const QString& pDeviceId);
		void timerEvent(QTimerEvent* event) override;
		void setScanRunning(bool pScanRunning);
		void onScanRunningChanged();

		QVector<QString> deviceIdsForReaderName(const QString& pReaderName);

	private Q_SLOTS:
		void onDeviceInitialized(const QBluetoothDeviceInfo& pInfo);
		void onDeviceDisconnected(const QBluetoothDeviceInfo& pInfo);
		void onDeviceDiscovered(const QBluetoothDeviceInfo& pInfo);
		void onDeviceDiscoveryFinished();
		void onDeviceDiscoveryError(QBluetoothDeviceDiscoveryAgent::Error pError);
		void onDeviceDiscoveryCanceled();
		void setBluetoothStatus(bool pEnabled);
		void onCardRemoved(const QString& pReaderName);
		void onReaderConnected(const QString& pReaderName);
		void onReaderConnectionFailed(const QString& pReaderName);

	public:
		BluetoothReaderManagerPlugIn();
		void init() override;
		QList<Reader*> getReaders() const override;

		virtual void startScan(bool pAutoConnect) override;
		virtual void stopScan(const QString& pError = QString()) override;
};

} // namespace governikus
