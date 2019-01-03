/*!
 * \brief Implementation of Reader object for Bluetooth based card reader.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "CyberJackWaveDevice.h"
#include "Reader.h"

#include <QBluetoothDeviceInfo>
#include <QLowEnergyController>


namespace governikus
{

class BluetoothReader
	: public ConnectableReader
{
	Q_OBJECT

	private:
		QSharedPointer<CyberJackWaveDevice> mDevice;
		CardEvent mLastCardEvent;
		QScopedPointer<Card> mCard;

		virtual CardEvent updateCard() override;
		void onCardRemoved();

	private Q_SLOTS:
		void onInitialized(const QBluetoothDeviceInfo& pInfo);
		void onDisconnected(const QBluetoothDeviceInfo& pInfo);
		void onError(QLowEnergyController::Error pError);
		void onStatusCharacteristicChanged(const QByteArray& pValue);

	Q_SIGNALS:
		void fireReaderConnected(const QString& pReaderName);
		void fireReaderConnectionFailed(const QString& pReaderName);

	public:
		BluetoothReader(const QSharedPointer<CyberJackWaveDevice>& pDevice);

		Card* getCard() const override;

		void connectReader() override;
		void disconnectReader() override;
};

} // namespace governikus
