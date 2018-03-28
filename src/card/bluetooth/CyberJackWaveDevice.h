/*!
 * \brief Implementation of a Reiner SCT cyberJack wave device.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include <QBluetoothDeviceInfo>
#include <QLowEnergyCharacteristic>
#include <QLowEnergyController>
#include <QLowEnergyService>


namespace governikus
{

class CyberJackWaveDevice
	: public QObject
{
	Q_OBJECT

	private:
		/*
		 * This is the maximum number of bytes arriving in one incoming response block
		 */
		static int INPUT_BLOCK_SIZE;

		/*
		 * This is the maximum number of bytes to send in one outgoing request block
		 */
		static int OUTPUT_BLOCK_SIZE;

		/*
		 * This is the service UUID of the LE service providing the "SIM Access Profile"
		 */
		static QBluetoothUuid SAP_SERVICE_UUID;

		/*
		 * This is the characteristic UUID of the SAP service for outgoing requests
		 */
		static QBluetoothUuid WRITE_CHARACTERISTIC_UUID;

		/*
		 * This is the characteristic UUID of the SAP service for incoming responses
		 */
		static QBluetoothUuid READ_CHARACTERISTIC_UUID;

		/*
		 * This is the characteristic UUID of the SAP service for status indications
		 */
		static QBluetoothUuid STATUS_CHARACTERISTIC_UUID;

		QBluetoothDeviceInfo mDeviceInfo;
		QLowEnergyController mLeController;
		QScopedPointer<QLowEnergyService> mSapService;
		QLowEnergyCharacteristic mStatusCharacteristic, mReadCharacteristic, mWriteCharacteristic;

		QLowEnergyService::WriteMode determineWriteMode(int pBlockIndex);

	private Q_SLOTS:
		void onConnectedDevice();
		void onDisconnectedDevice();
		void onDeviceError(QLowEnergyController::Error pError);
		void onDeviceStateChanged(QLowEnergyController::ControllerState pState);
		void onServiceDiscoveryFinished();
		void onServiceStateChanged(QLowEnergyService::ServiceState pState);
		void onServiceError(QLowEnergyService::ServiceError pError);
		void onCharacteristicChanged(const QLowEnergyCharacteristic& pCharacteristic, const QByteArray& pNewValue);

	public:
		CyberJackWaveDevice(const QBluetoothDeviceInfo& pDeviceInfo);
		~CyberJackWaveDevice();

		QString getName() const;
		void setDeviceInfo(const QBluetoothDeviceInfo& pInfo);

		bool isValid() const;

		bool write(const QByteArray& pValue);

	public Q_SLOTS:
		/*!
		 * Connect to the device and initialize.
		 */
		void initialize();

		/*!
		 * Disconnect from the device.
		 */
		void disconnectFromDevice();

	Q_SIGNALS:
		void fireInitialized(const QBluetoothDeviceInfo& pInfo);
		void fireDisconnected(const QBluetoothDeviceInfo& pInfo);
		void fireError(QLowEnergyController::Error pError);
		void fireReadCharacteristicChanged(const QByteArray& pValue);
		void fireStatusCharacteristicChanged(const QByteArray& pValue);
};

} /* namespace governikus */
