/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "SynchronousBtCall.h"

#include <QLoggingCategory>
#include <QTimer>

using namespace governikus;

Q_DECLARE_LOGGING_CATEGORY(bluetooth)

SynchronousBtCall::SynchronousBtCall(const QSharedPointer<CyberJackWaveDevice>& pDevice)
	: QObject()
	, mDevice(pDevice)
	, mEventLoop()
	, mBuffer()
	, mMessage()
{
	connect(mDevice.data(), &CyberJackWaveDevice::fireReadCharacteristicChanged, this, &SynchronousBtCall::onCharacteristicChanged);
}


QSharedPointer<const BluetoothMessage> SynchronousBtCall::send(const BluetoothMessage& pRequest, BluetoothMsgId pResponseType, quint8 pTimeoutSeconds)
{
	Q_ASSERT(pTimeoutSeconds > 0);
	QTimer::singleShot(pTimeoutSeconds * 1000, &mEventLoop, &QEventLoop::quit);
	mDevice->write(pRequest.toData());
	mEventLoop.exec();

	if (mMessage.isNull())
	{
		qCCritical(bluetooth) << "No response received for" << pRequest.getBluetoothMsgId();
		return QSharedPointer<const BluetoothMessage>();
	}
	else if (mMessage->getBluetoothMsgId() != pResponseType)
	{
		qCCritical(bluetooth) << "Got unexpected response type" << mMessage->getBluetoothMsgId();
		return QSharedPointer<const BluetoothMessage>();
	}

	return mMessage;
}


void SynchronousBtCall::onCharacteristicChanged(const QByteArray& pNewValue)
{
	{
		mBuffer += pNewValue;
		auto messages = BluetoothMessageParser(mBuffer).getMessages();
		if (!messages.isEmpty())
		{
			qCDebug(bluetooth) << "Bluetooth message complete";
			mMessage = messages.at(0);
			mEventLoop.quit();
		}
	}
}
