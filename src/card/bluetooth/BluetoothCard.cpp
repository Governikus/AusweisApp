/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothCard.h"

#include "DestroyPaceChannel.h"
#include "EstablishPaceChannel.h"
#include "messages/BluetoothMessageCreator.h"
#include "messages/BluetoothMessageSetTransportProtocolResponse.h"
#include "messages/BluetoothMessageTransferApduResponse.h"
#include "PinModify.h"
#include "PinModifyOutput.h"
#include "SynchronousBtCall.h"


#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(bluetooth)


using namespace governikus;


BluetoothCard::BluetoothCard(QSharedPointer<CyberJackWaveDevice> pDevice)
	: Card()
	, mConnected(false)
	, mDevice(pDevice)
{
}


CardReturnCode BluetoothCard::connect()
{
	if (isConnected())
	{
		qCCritical(bluetooth) << "Card is already connected";
		return CardReturnCode::COMMAND_FAILED;
	}

	auto request = BluetoothMessageCreator::createSetTransportProtocolRequest(BluetoothTransportProtocol::T1);
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::SetTransportProtocolResponse);
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		return CardReturnCode::COMMAND_FAILED;
	}

	auto resultCode = response.staticCast<const BluetoothMessageSetTransportProtocolResponse>()->getResultCode();
	if (resultCode != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "Cannot set transport protocol on card" << resultCode;
		return CardReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	return CardReturnCode::OK;
}


CardReturnCode BluetoothCard::disconnect()
{
	if (!isConnected())
	{
		qCCritical(bluetooth) << "Card is already disconnected";
		return CardReturnCode::COMMAND_FAILED;
	}

	auto request = BluetoothMessageCreator::createDisconnectRequest();
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::DisconnectResponse, 1);
	mConnected = false;
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		return CardReturnCode::COMMAND_FAILED;
	}

	return CardReturnCode::OK;
}


bool BluetoothCard::isConnected()
{
	return mConnected;
}


CardReturnCode BluetoothCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	return transmit(pCmd, pRes, 30);
}


CardReturnCode BluetoothCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes, quint8 pTimeoutSeconds)
{
	if (!isConnected())
	{
		qCCritical(bluetooth) << "Card is not connected, abort transmit";
		return CardReturnCode::COMMAND_FAILED;
	}

	qCDebug(bluetooth) << "Transmit command APDU:" << pCmd.getBuffer().toHex();
	auto request = BluetoothMessageCreator::createTransferApduRequest(pCmd.getBuffer());
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::TransferApduResponse, pTimeoutSeconds);
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		return CardReturnCode::COMMAND_FAILED;
	}

	auto apduResponse = response.staticCast<const BluetoothMessageTransferApduResponse>();
	if (apduResponse->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "TransferApduResponse failed:" << apduResponse->getResultCode();
		return CardReturnCode::COMMAND_FAILED;
	}
	pRes.setBuffer(apduResponse->getResponseAPDU());
	qCDebug(bluetooth) << "Transmit response APDU:" << pRes.getBuffer().toHex();
	return CardReturnCode::OK;
}


CardReturnCode BluetoothCard::establishPaceChannel(PacePasswordId pPasswordId,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription,
		EstablishPaceChannelOutput& pChannelOutput,
		quint8 pTimeoutSeconds)
{
	EstablishPaceChannel builder;
	builder.setPasswordId(pPasswordId);
	builder.setChat(pChat);
	builder.setCertificateDescription(pCertificateDescription);

	ResponseApdu response;

	CardReturnCode returnCode = transmit(builder.createCommandDataCcid(), response, pTimeoutSeconds);
	if (returnCode != CardReturnCode::OK)
	{
		return returnCode;
	}

	pChannelOutput.parseFromCcid(response.getBuffer(), pPasswordId);
	return pChannelOutput.getPaceReturnCode();
}


CardReturnCode BluetoothCard::destroyPaceChannel()
{
	DestroyPaceChannelBuilder builder;
	ResponseApdu response;
	return transmit(builder.createCommandDataCcid(), response);
}


CardReturnCode BluetoothCard::setEidPin(quint8 pTimeoutSeconds, ResponseApdu& pResponseApdu)
{
	PinModify pinModify(pTimeoutSeconds);
	CommandApdu command = pinModify.createCcidForBluetooth();

	CardReturnCode returnCode = transmit(command, pResponseApdu, pTimeoutSeconds);
	if (returnCode != CardReturnCode::OK)
	{
		return returnCode;
	}

	PinModifyOutput output(pResponseApdu);
	return output.getReturnCode();
}
