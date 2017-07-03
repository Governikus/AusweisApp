/*!
 * SynchronousBtCall.h
 *
 * \brief Helper class to make a synchronous call to a Bluetooth device, i.e.
 * send the request data and wait for the response data to arrive.
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#pragma once


#include "CyberJackWaveDevice.h"
#include "messages/BluetoothMessage.h"
#include "messages/BluetoothMessageParser.h"


#include <QEventLoop>
#include <QLoggingCategory>
#include <QSharedPointer>
#include <QTimer>


Q_DECLARE_LOGGING_CATEGORY(bluetooth)


namespace governikus
{

class SynchronousBtCall
	: public QObject
{
	Q_OBJECT

	private:
		QSharedPointer<CyberJackWaveDevice> mDevice;
		QEventLoop mEventLoop;
		QByteArray mBuffer;
		QSharedPointer<const BluetoothMessage> mMessage;

	public:
		SynchronousBtCall(QSharedPointer<CyberJackWaveDevice> pDevice)
			: QObject()
			, mDevice(pDevice)
			, mEventLoop()
			, mBuffer()
			, mMessage()
		{
			connect(mDevice.data(), &CyberJackWaveDevice::fireReadCharacteristicChanged, this, &SynchronousBtCall::onCharacteristicChanged);
		}


		template<typename T> QSharedPointer<const T> send(const BluetoothMessage& pRequest, int pTimeoutSeconds = 20)
		{
			Q_ASSERT(pTimeoutSeconds > 0);
			QTimer::singleShot(pTimeoutSeconds * 1000, &mEventLoop, &QEventLoop::quit);
			mDevice->write(pRequest.toData());
			mEventLoop.exec();

			if (mMessage == nullptr)
			{
				qCCritical(bluetooth) << "No response received for " << pRequest.getBluetoothMsgId();
				return QSharedPointer<const T>();
			}

			QSharedPointer<const T> response = mMessage.dynamicCast<const T>();
			if (response == nullptr)
			{
				qCCritical(bluetooth) << "Got unexpected response type " << mMessage->getBluetoothMsgId();
			}
			return response;
		}


	private Q_SLOTS:
		void onCharacteristicChanged(const QByteArray& pNewValue)
		{
			mBuffer.append(pNewValue);
			auto messages = BluetoothMessageParser(mBuffer).getMessages();
			if (!messages.isEmpty())
			{
				qCDebug(bluetooth) << "Bluetooth message complete";
				mMessage = messages.at(0);
				mEventLoop.quit();
			}
		}


};

} /* namespace governikus */
