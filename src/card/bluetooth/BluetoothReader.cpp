/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothCard.h"
#include "BluetoothDebug.h"
#include "BluetoothReader.h"
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
	: ConnectableReader(ReaderManagerPlugInType::BLUETOOTH, pDevice->getName())
	, mDevice(pDevice)
	, mLastCardEvent(CardEvent::NONE)
	, mCard()
{
	mReaderInfo.setBasicReader(false);
	connect(mDevice.data(), &CyberJackWaveDevice::fireInitialized, this, &BluetoothReader::onInitialized);
	connect(mDevice.data(), &CyberJackWaveDevice::fireDisconnected, this, &BluetoothReader::onDisconnected);
	connect(mDevice.data(), &CyberJackWaveDevice::fireError, this, &BluetoothReader::onError);
	connect(mDevice.data(), &CyberJackWaveDevice::fireStatusCharacteristicChanged, this, &BluetoothReader::onStatusCharacteristicChanged);
	mReaderInfo.setConnected(mDevice->isValid());
	qCDebug(bluetooth) << "Created reader" << getName() << "with connected status:" << mReaderInfo.isConnected();
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
	const QString& name = mReaderInfo.getName();
	qCDebug(bluetooth) << "Connected reader" << name << "is valid:" << mDevice->isValid();

	/*
	 * Attention: This also triggers the pairing!
	 * (Pairing is initiated by the device on the first write request with WriteMode::WriteWithResponse.)
	 */
	auto request = BluetoothMessageCreator::createSetTransportProtocolRequest(BluetoothTransportProtocol::T1);
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::SetTransportProtocolResponse);
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		Q_EMIT fireReaderConnectionFailed(name);
		return;
	}

	auto protocolResponse = response.staticCast<const BluetoothMessageSetTransportProtocolResponse>();
	if (protocolResponse->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "Error setting transport protocol";
		Q_EMIT fireReaderConnectionFailed(name);
		return;
	}

	Q_EMIT fireReaderConnected(name);

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
		Q_EMIT fireReaderDeviceError(GlobalStatus::Code::Workflow_Reader_Device_Connection_Error);
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
	mReaderInfo.setCardInfo(CardInfo(CardType::NONE));
	mCard.reset();
}
