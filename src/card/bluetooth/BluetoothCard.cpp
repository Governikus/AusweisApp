/*!
 * BluetoothCard.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */
#include "BluetoothCard.h"
#include "DestroyPACEChannel.h"
#include "messages/BluetoothMessageCreator.h"
#include "messages/BluetoothMessageDisconnectResponse.h"
#include "messages/BluetoothMessageSetTransportProtocolResponse.h"
#include "messages/BluetoothMessageTransferApduResponse.h"
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


ReturnCode BluetoothCard::connect()
{
	if (isConnected())
	{
		qCCritical(bluetooth) << "Card is already connected";
		return ReturnCode::COMMAND_FAILED;
	}

	auto request = BluetoothMessageCreator::createSetTransportProtocolRequest(BluetoothTransportProtocol::T1);
	auto response = SynchronousBtCall(mDevice).send<BluetoothMessageSetTransportProtocolResponse>(request);
	if (response == nullptr)
	{
		qCCritical(bluetooth) << "Response is empty";
		return ReturnCode::COMMAND_FAILED;
	}
	else if (response->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "Cannot set transport protocol on card" << response->getResultCode();
		return ReturnCode::COMMAND_FAILED;
	}

	mConnected = true;
	return ReturnCode::OK;
}


ReturnCode BluetoothCard::disconnect()
{
	if (!isConnected())
	{
		qCCritical(bluetooth) << "Card is already disconnected";
		return ReturnCode::COMMAND_FAILED;
	}

	auto request = BluetoothMessageCreator::createDisconnectRequest();
	auto response = SynchronousBtCall(mDevice).send<BluetoothMessageDisconnectResponse>(request, 1);
	mConnected = false;
	if (response == nullptr)
	{
		qCCritical(bluetooth) << "Response is empty";
		return ReturnCode::COMMAND_FAILED;
	}

	return ReturnCode::OK;
}


bool BluetoothCard::isConnected()
{
	return mConnected;
}


ReturnCode BluetoothCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes)
{
	return transmit(pCmd, pRes, 30);
}


ReturnCode BluetoothCard::transmit(const CommandApdu& pCmd, ResponseApdu& pRes, unsigned int pTimeoutSeconds)
{
	if (!isConnected())
	{
		qCCritical(bluetooth) << "Card is not connected, abort transmit";
		return ReturnCode::COMMAND_FAILED;
	}

	qCDebug(bluetooth) << "Transmit command APDU: " << pCmd.getBuffer().toHex();
	auto request = BluetoothMessageCreator::createTransferApduRequest(pCmd.getBuffer());
	auto response = SynchronousBtCall(mDevice).send<BluetoothMessageTransferApduResponse>(request, pTimeoutSeconds);
	if (response == nullptr)
	{
		qCCritical(bluetooth) << "Response is empty";
		return ReturnCode::COMMAND_FAILED;
	}
	if (response->getResultCode() != BluetoothResultCode::Ok)
	{
		qCCritical(bluetooth) << "TransferApduResponse failed : " << response->getResultCode();
		return ReturnCode::COMMAND_FAILED;
	}
	pRes.setBuffer(response->getResponseAPDU());
	qCDebug(bluetooth) << "Transmit response APDU: " << pRes.getBuffer().toHex();
	return ReturnCode::OK;
}


ReturnCode BluetoothCard::establishPaceChannel(PACE_PIN_ID pPinId,
		const QByteArray& pChat,
		const QByteArray& pCertificateDescription,
		EstablishPACEChannelOutput& pChannelOutput,
		int pTimeoutSeconds)
{
	EstablishPACEChannelBuilder builder;
	builder.setPinId(pPinId);
	builder.setChat(pChat);
	builder.setCertificateDescription(pCertificateDescription);

	ResponseApdu response;

	ReturnCode returnCode = transmit(builder.createCommandDataCcid(), response, pTimeoutSeconds);
	if (returnCode != ReturnCode::OK)
	{
		return returnCode;
	}

	pChannelOutput.parseFromCcid(response.getBuffer(), pPinId);
	return pChannelOutput.getPaceReturnCode();
}


ReturnCode BluetoothCard::destroyPaceChannel()
{
	DestroyPACEChannelBuilder builder;
	ResponseApdu response;
	return transmit(builder.createCommandDataCcid(), response);
}


ReturnCode BluetoothCard::setEidPin(unsigned int pTimeoutSeconds)
{
	PinModifyBuilder builder;
	CommandApdu command = builder.createCommandDataCcid(pTimeoutSeconds);

	ResponseApdu response;
	ReturnCode returnCode = transmit(command, response, pTimeoutSeconds);
	if (returnCode != ReturnCode::OK)
	{
		return returnCode;
	}

	PinModifyOutput output;
	output.parseFromCcid(response.getBuffer());
	return output.getReturnCode();
}
