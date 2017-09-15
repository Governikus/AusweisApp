/*!
 * BluetoothReader.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "BluetoothCard.h"
#include "BluetoothDebug.h"
#include "BluetoothReader.h"
#include "DeviceError.h"
#include "messages/BluetoothMessageCreator.h"
#include "messages/BluetoothMessageParser.h"
#include "messages/BluetoothMessageSetTransportProtocolResponse.h"
#include "messages/BluetoothMessageStatusInd.h"
#include "SynchronousBtCall.h"

#include <QDebug>
#include <QLoggingCategory>
#include <QSignalBlocker>

Q_DECLARE_LOGGING_CATEGORY(bluetooth)


using namespace governikus;


BluetoothReader::BluetoothReader(const QSharedPointer<CyberJackWaveDevice>& pDevice)
	: ConnectableReader(ReaderManagerPlugInType::BLUETOOTH, pDevice->getName(), ReaderType::REINER_cyberJack_wave)
	, mDevice(pDevice)
	, mLastCardEvent(CardEvent::NONE)
	, mCard()
{
	mReaderInfo.setBasicReader(false);
	mReaderInfo.setConnected(false);
	connect(mDevice.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReader::onInitialized);
	connect(mDevice.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReader::onDisconnected);
	connect(mDevice.data(), &CyberJackWaveDevice::fireError, this, &BluetoothReader::onError);
	connect(mDevice.data(), &CyberJackWaveDevice::fireStatusCharacteristicChanged, this, &BluetoothReader::onStatusCharacteristicChanged);
}


Card* BluetoothReader::getCard() const
{
	return mCard.data();
}


void BluetoothReader::connectReader()
{
	mDevice->initialize();
}


void BluetoothReader::onInitialized(const QBluetoothDeviceInfo&)
{
	qCDebug(bluetooth) << "Connected reader" << getName() << "is valid:" << mDevice->isValid();

	/*
	 * Attention: This also triggers the pairing!
	 * (Pairing is initiated by the device on the first write request with WriteMode::WriteWithResponse.)
	 */
	auto request = BluetoothMessageCreator::createSetTransportProtocolRequest(BluetoothTransportProtocol::T1);
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::SetTransportProtocolResponse);
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		return;
	}

	auto protocolResponse = response.staticCast<const BluetoothMessageSetTransportProtocolResponse>();
	if (protocolResponse->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "Error setting transport protocol";
		return;
	}

	Q_EMIT fireReaderConnected(mReaderInfo.getName());

	mReaderInfo.setConnected(mDevice->isValid());
	mLastCardEvent = CardEvent::CARD_REMOVED;
	mTimerId = startTimer(500);
}


void BluetoothReader::disconnectReader()
{
	mDevice->disconnectFromDevice();
}


void BluetoothReader::onDisconnected(const QBluetoothDeviceInfo&)
{
	qCDebug(bluetooth) << "Disconnected reader" << getName();

	killTimer(mTimerId);
	mTimerId = 0;
	mReaderInfo.setConnected(false);

	/*
	 * We remove the card, because the user may remove it either when the reader is disconnected.
	 * Finally we perform one update manually, because the timer is already stopped.
	 */
	onCardRemoved();
	update();
}


void BluetoothReader::onError(QLowEnergyController::Error pError)
{
	if (pError == QLowEnergyController::ConnectionError)
	{
		Q_EMIT fireReaderDeviceError(DeviceError::DEVICE_CONNECTION_ERROR);
	}
}


Reader::CardEvent BluetoothReader::updateCard()
{
	CardEvent tmpEvent = mLastCardEvent;
	mLastCardEvent = CardEvent::NONE;
	return tmpEvent;
}


void BluetoothReader::onStatusCharacteristicChanged(const QByteArray& pValue)
{
	auto messages = BluetoothMessageParser(pValue).getMessages();
	if (messages.size() != 1 || messages.at(0)->getBluetoothMsgId() != BluetoothMsgId::StatusInd)
	{
		qCCritical(card) << "Cannot handle Bluetooth message";
		return;
	}

	auto statusChange = messages.at(0).staticCast<const BluetoothMessageStatusInd>()->getStatusChange();
	if (mCard.isNull() && (statusChange == BluetoothStatusChange::CardInserted || statusChange == BluetoothStatusChange::CardReset))
	{
		qCDebug(card) << "Card inserted" << getName();
		mCard.reset(new BluetoothCard(mDevice));
		QSharedPointer<CardConnectionWorker> cardConnection = createCardConnectionWorker();
		CardInfoFactory::create(cardConnection, mReaderInfo);
		const QSignalBlocker blocker(this);
		updateRetryCounter(cardConnection);
		mLastCardEvent = CardEvent::CARD_INSERTED;
	}
	else if (!mCard.isNull() && statusChange == BluetoothStatusChange::CardRemoved)
	{
		onCardRemoved();
	}
	else
	{
		qCWarning(card) << "Got unhandled card reader status" << statusChange;
	}
}


void BluetoothReader::onCardRemoved()
{
	qCDebug(card) << "Card removed" << getName();
	mLastCardEvent = CardEvent::CARD_REMOVED;
	mUpdateRetryCounter = false;
	mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
	mCard.reset();
}
