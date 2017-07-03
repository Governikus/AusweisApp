/*!
 * CyberJackWaveDevice.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "BluetoothDebug.h"
#include "BluetoothReader.h"
#include "CyberJackWaveDevice.h"
#include "NotificationEnabler.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(bluetooth)

int CyberJackWaveDevice::INPUT_BLOCK_SIZE = 19;
int CyberJackWaveDevice::OUTPUT_BLOCK_SIZE = 20;
QBluetoothUuid CyberJackWaveDevice::SAP_SERVICE_UUID = QBluetoothUuid(QStringLiteral("77C2DAA3-44DA-45DC-B476-7495EA5C9842"));
QBluetoothUuid CyberJackWaveDevice::WRITE_CHARACTERISTIC_UUID = QBluetoothUuid(QStringLiteral("71E11650-718E-4FB2-B0B8-4E71A78AB88F"));
QBluetoothUuid CyberJackWaveDevice::READ_CHARACTERISTIC_UUID = QBluetoothUuid(QStringLiteral("18EA75F5-F9A1-41CA-9958-F5959C903C2F"));
QBluetoothUuid CyberJackWaveDevice::STATUS_CHARACTERISTIC_UUID = QBluetoothUuid(QStringLiteral("B88BBD3F-0313-47DA-8258-3E46F4D645B6"));


CyberJackWaveDevice::CyberJackWaveDevice(const QBluetoothDeviceInfo& pDeviceInfo)
	: QObject()
	, mDeviceInfo(pDeviceInfo)
	, mLeController(pDeviceInfo)
	, mSapService()
	, mStatusCharacteristic()
	, mReadCharacteristic()
	, mWriteCharacteristic()
{
	connect(&mLeController, &QLowEnergyController::connected, this, &CyberJackWaveDevice::onConnectedDevice);
	connect(&mLeController, &QLowEnergyController::disconnected, this, &CyberJackWaveDevice::onDisconnectedDevice);
	connect(&mLeController, QOverload<QLowEnergyController::Error>::of(&QLowEnergyController::error), this, &CyberJackWaveDevice::onDeviceError);
	connect(&mLeController, &QLowEnergyController::discoveryFinished, this, &CyberJackWaveDevice::onServiceDiscoveryFinished);
	connect(&mLeController, &QLowEnergyController::stateChanged, this, &CyberJackWaveDevice::onDeviceStateChanged);
}


CyberJackWaveDevice::~CyberJackWaveDevice()
{
	if (mLeController.state() != QLowEnergyController::ClosingState && mLeController.state() != QLowEnergyController::UnconnectedState)
	{
		qCDebug(bluetooth) << "Disconnecting from device";
		mLeController.disconnectFromDevice();
	}
}


bool CyberJackWaveDevice::isValid() const
{
	return mStatusCharacteristic.isValid() && mReadCharacteristic.isValid() && mWriteCharacteristic.isValid();
}


QString CyberJackWaveDevice::getName() const
{
	return mDeviceInfo.name();
}


void CyberJackWaveDevice::setDeviceInfo(const QBluetoothDeviceInfo& pInfo)
{
	mDeviceInfo = pInfo;
}


void CyberJackWaveDevice::initialize()
{
	if (!(mDeviceInfo.coreConfigurations() & QBluetoothDeviceInfo::LowEnergyCoreConfiguration))
	{
		qCDebug(bluetooth) << "It's not a Bluetooth LE device, skip further processing";
		Q_EMIT fireInitialized(mDeviceInfo);
		return;
	}

	if (!mDeviceInfo.name().startsWith(QLatin1String("cyberJack wave")))
	{
		qCDebug(bluetooth) << "It's not a cyberJack wave device, skip further processing";
		Q_EMIT fireInitialized(mDeviceInfo);
		return;
	}


	qCDebug(bluetooth) << "Connecting device" << mDeviceInfo;
	mLeController.connectToDevice();
}


void CyberJackWaveDevice::onConnectedDevice()
{
	qCDebug(bluetooth) << "Connected device" << mDeviceInfo;
	if (mLeController.state() == QLowEnergyController::DiscoveredState)
	{
		onServiceDiscoveryFinished();
	}
	else
	{
		qCDebug(bluetooth) << "Start service discovery for device" << mDeviceInfo;
		mLeController.discoverServices();
	}
}


void CyberJackWaveDevice::onServiceDiscoveryFinished()
{
	qCDebug(bluetooth) << "Services discovered for device" << mDeviceInfo;
	for (const auto& serviceUuid : mLeController.services())
	{
		qCDebug(bluetooth) << "Found service" << serviceUuid;
		if (const auto service = QSharedPointer<QLowEnergyService>(mLeController.createServiceObject(serviceUuid)))
		{
			for (const auto& characteristic : service->characteristics())
			{
				qCDebug(bluetooth) << "Found service characteristic" << characteristic.uuid();
				for (const auto& descriptor : characteristic.descriptors())
				{
					qCDebug(bluetooth) << "Found service characteristic descriptor" << descriptor.uuid();
				}
			}
		}
	}

	if (mLeController.services().contains(SAP_SERVICE_UUID))
	{
		qCDebug(bluetooth) << "Found SAP service" << SAP_SERVICE_UUID;
		mSapService.reset(mLeController.createServiceObject(SAP_SERVICE_UUID));

		switch (mSapService->state())
		{
			case QLowEnergyService::DiscoveryRequired:
			{
				connect(mSapService.data(), &QLowEnergyService::stateChanged, this, &CyberJackWaveDevice::onServiceStateChanged);
				connect(mSapService.data(), &QLowEnergyService::characteristicChanged, this, &CyberJackWaveDevice::onCharacteristicChanged);
				connect(mSapService.data(), QOverload<QLowEnergyService::ServiceError>::of(&QLowEnergyService::error), this, &CyberJackWaveDevice::onServiceError);
				mSapService->discoverDetails();
				break;
			}

			default:
				onServiceStateChanged(mSapService->state());
				break;
		}
		return;
	}
}


void CyberJackWaveDevice::onServiceStateChanged(QLowEnergyService::ServiceState pState)
{
	qCDebug(bluetooth) << "Service state changed" << pState;
	if (pState == QLowEnergyService::ServiceDiscovered)
	{
		mReadCharacteristic = mSapService->characteristic(READ_CHARACTERISTIC_UUID);
		mWriteCharacteristic = mSapService->characteristic(WRITE_CHARACTERISTIC_UUID);
		mStatusCharacteristic = mSapService->characteristic(STATUS_CHARACTERISTIC_UUID);

		NotificationEnabler(mSapService.data()).enable(mReadCharacteristic);
		NotificationEnabler(mSapService.data()).enable(mStatusCharacteristic);

		Q_EMIT fireInitialized(mDeviceInfo);
	}
}


void CyberJackWaveDevice::onServiceError(QLowEnergyService::ServiceError pError)
{
	qCCritical(bluetooth) << "A service error occurred" << pError;
}


void CyberJackWaveDevice::onDisconnectedDevice()
{
	qCDebug(bluetooth) << "Disconnected device" << mDeviceInfo;
	Q_EMIT fireDisconnected(mDeviceInfo);
}


void CyberJackWaveDevice::onDeviceStateChanged(QLowEnergyController::ControllerState pState)
{
	qCDebug(bluetooth) << "State changed" << pState << "for device" << mDeviceInfo;
}


void CyberJackWaveDevice::onDeviceError(QLowEnergyController::Error pError)
{
	qCDebug(bluetooth) << "Error" << pError << mLeController.errorString() << "on device" << mDeviceInfo;
	Q_EMIT fireError(pError);
}


void CyberJackWaveDevice::disconnectFromDevice()
{
	if (isValid())
	{
		NotificationEnabler(mSapService.data(), 1).disable(mReadCharacteristic);
		NotificationEnabler(mSapService.data(), 1).disable(mStatusCharacteristic);
	}
	mWriteCharacteristic = QLowEnergyCharacteristic(); // invalidate characteristic
	mLeController.disconnectFromDevice();
}


bool CyberJackWaveDevice::write(const QByteArray& pValue)
{
	if (!isValid())
	{
		qCCritical(bluetooth) << "Device is invalid, abort writing";
		return false;
	}

	for (int start = 0, blockIndex = 0; start <= pValue.length(); start += OUTPUT_BLOCK_SIZE, ++blockIndex)
	{
		QLowEnergyService::WriteMode mode = determineWriteMode(blockIndex);
		QByteArray block = pValue.mid(start, OUTPUT_BLOCK_SIZE);

		qCDebug(bluetooth) << "Write block to characteristic" << block.toHex() << mode;
		mSapService->writeCharacteristic(mWriteCharacteristic, block, mode);
	}
	return true;
}


QLowEnergyService::WriteMode CyberJackWaveDevice::determineWriteMode(int pBlockIndex)
{
	/*
	 * Make sure the first block will be send with confirmation response, otherwise the Bluetooth pairing will not be performed!
	 */

#ifdef Q_OS_IOS

	/*
	 * This information is according to Reiner SCT:
	 *
	 * Writing to a characteristic WITHOUT confirmation response, i.e. with
	 * WriteMode::WriteWithoutResponse is twelve times faster than
	 * WITH confirmation response, i.e. with WriteMode::WriteWithResponse.
	 *
	 * But writing lots of data without confirmation response may cause data
	 * loss, so every 6th block written to the characteristic must be with
	 * confirmation response.
	 */
	static int IOS_CONFIRMATION_BLOCK_NUMBER = 6;
	return pBlockIndex % IOS_CONFIRMATION_BLOCK_NUMBER ? QLowEnergyService::WriteWithoutResponse : QLowEnergyService::WriteWithResponse;

#else
	Q_UNUSED(pBlockIndex);

	/*
	 * According to Reiner SCT on Android the behaviour is too device specific,
	 * so always send with confirmation response.
	 */
	return QLowEnergyService::WriteWithResponse;

#endif
}


void CyberJackWaveDevice::onCharacteristicChanged(const QLowEnergyCharacteristic& pCharacteristic,
		const QByteArray& pNewValue)
{
	if (pCharacteristic.uuid() == READ_CHARACTERISTIC_UUID)
	{
		qCDebug(bluetooth) << "READ characteristic changed" << pNewValue.toHex();
		Q_EMIT fireReadCharacteristicChanged(pNewValue);
	}
	else if (pCharacteristic.uuid() == STATUS_CHARACTERISTIC_UUID)
	{
		qCDebug(bluetooth) << "STATUS characteristic changed" << pNewValue.toHex();
		Q_EMIT fireStatusCharacteristicChanged(pNewValue);
	}
	else
	{
		qCDebug(bluetooth) << "Unknown characteristic" << pCharacteristic.uuid() << "changed" << pNewValue.toHex();
	}
}
