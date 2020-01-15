/*!
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
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


ResponseApduResult BluetoothCard::transmit(const CommandApdu& pCmd)
{
	return transmit(pCmd, 30);
}


ResponseApduResult BluetoothCard::transmit(const CommandApdu& pCmd, quint8 pTimeoutSeconds)
{
	if (!isConnected())
	{
		qCCritical(bluetooth) << "Card is not connected, abort transmit";
		return {CardReturnCode::COMMAND_FAILED};
	}

	qCDebug(bluetooth) << "Transmit command APDU:" << pCmd.getBuffer().toHex();
	auto request = BluetoothMessageCreator::createTransferApduRequest(pCmd.getBuffer());
	auto response = SynchronousBtCall(mDevice).send(request, BluetoothMsgId::TransferApduResponse, pTimeoutSeconds);
	if (response.isNull())
	{
		qCCritical(bluetooth) << "Response is empty";
		return {CardReturnCode::COMMAND_FAILED};
	}

	auto apduResponse = response.staticCast<const BluetoothMessageTransferApduResponse>();
	if (apduResponse->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "TransferApduResponse failed:" << apduResponse->getResultCode();
		return {CardReturnCode::COMMAND_FAILED};
	}

	const ResponseApdu resp(apduResponse->getResponseAPDU());
	qCDebug(bluetooth) << "Transmit response APDU:" << resp.getBuffer().toHex();
	return {CardReturnCode::OK, resp};
}


EstablishPaceChannelOutput BluetoothCard::establishPaceChannel(PacePasswordId pPasswordId,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription,
		quint8 pTimeoutSeconds)
{
	EstablishPaceChannel builder;
	builder.setPasswordId(pPasswordId);
	builder.setChat(pChat);
	builder.setCertificateDescription(pCertificateDescription);

	auto [returnCode, response] = transmit(builder.createCommandDataCcid(), pTimeoutSeconds);
	if (returnCode != CardReturnCode::OK)
	{
		return returnCode;
	}

	EstablishPaceChannelOutput output;
	output.parseFromCcid(response.getBuffer(), pPasswordId);
	return output;
}


CardReturnCode BluetoothCard::destroyPaceChannel()
{
	DestroyPaceChannelBuilder builder;
	return transmit(builder.createCommandDataCcid()).mReturnCode;
}


ResponseApduResult BluetoothCard::setEidPin(quint8 pTimeoutSeconds)
{
	PinModify pinModify(pTimeoutSeconds);
	CommandApdu command = pinModify.createCcidForBluetooth();

	auto [returnCode, response] = transmit(command, pTimeoutSeconds);
	if (returnCode != CardReturnCode::OK)
	{
		return {returnCode, response};
	}

	const PinModifyOutput output(response);
	return {output.getReturnCode(), response};
}
