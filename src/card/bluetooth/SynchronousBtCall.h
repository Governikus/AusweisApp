/*!
 * \brief Helper class to make a synchronous call to a Bluetooth device, i.e.
 * send the request data and wait for the response data to arrive.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CyberJackWaveDevice.h"
#include "messages/BluetoothMessage.h"
#include "messages/BluetoothMessageParser.h"

#include <QEventLoop>
#include <QSharedPointer>

namespace governikus
{

class SynchronousBtCall
	: public QObject
{
	Q_OBJECT

	private:
		const QSharedPointer<CyberJackWaveDevice> mDevice;
		QEventLoop mEventLoop;
		QByteArray mBuffer;
		QSharedPointer<const BluetoothMessage> mMessage;

	public:
		SynchronousBtCall(const QSharedPointer<CyberJackWaveDevice>& pDevice);
		QSharedPointer<const BluetoothMessage> send(const BluetoothMessage& pRequest, BluetoothMsgId pResponseType, quint8 pTimeoutSeconds = 20);

	private Q_SLOTS:
		void onCharacteristicChanged(const QByteArray& pNewValue);
};

} // namespace governikus
