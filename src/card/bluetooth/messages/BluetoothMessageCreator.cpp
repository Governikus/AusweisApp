/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */


#include "BluetoothMessageCreator.h"
#include "parameter/BluetoothMessageParameterMaxMsgSize.h"


#include <QDataStream>


using namespace governikus;


BluetoothMessageConnectRequest BluetoothMessageCreator::createConnectRequest(uint pMaxMsgSize)
{
	BluetoothMessageConnectRequest msg;
	msg.copyParameter(BluetoothMessageParameterMaxMsgSize(pMaxMsgSize));
	return msg;
}


BluetoothMessageDisconnectRequest BluetoothMessageCreator::createDisconnectRequest()
{
	return BluetoothMessageDisconnectRequest();
}


BluetoothMessageTransferApduRequest BluetoothMessageCreator::createTransferApduRequest(const QByteArray& pApdu)
{
	BluetoothMessageTransferApduRequest msg;
	msg.copyParameter(BluetoothMessageParameter(BluetoothParamId::CommandAPDU7816, pApdu));
	return msg;
}


BluetoothMessageTransferAtrRequest BluetoothMessageCreator::createTransferAtrRequest()
{
	return BluetoothMessageTransferAtrRequest();
}


BluetoothMessagePowerSimOnRequest BluetoothMessageCreator::createPowerSimOnRequest()
{
	return BluetoothMessagePowerSimOnRequest();
}


BluetoothMessagePowerSimOffRequest BluetoothMessageCreator::createPowerSimOffRequest()
{
	return BluetoothMessagePowerSimOffRequest();
}


BluetoothMessageResetSimRequest BluetoothMessageCreator::createResetSimRequest()
{
	return BluetoothMessageResetSimRequest();
}


BluetoothMessageTransferCardReaderStatusRequest BluetoothMessageCreator::createTransferCardReaderStatusRequest()
{
	return BluetoothMessageTransferCardReaderStatusRequest();
}


BluetoothMessageSetTransportProtocolRequest BluetoothMessageCreator::createSetTransportProtocolRequest(BluetoothTransportProtocol pProtocol)
{
	BluetoothMessageSetTransportProtocolRequest msg;
	msg.copyParameter(BluetoothMessageParameter(BluetoothParamId::TransportProtocol, QByteArray(1, Enum<BluetoothTransportProtocol>::getValue(pProtocol))));
	return msg;
}


BluetoothMessageConnectRequest::BluetoothMessageConnectRequest()
	: BluetoothMessage(BluetoothMsgId::ConnectRequest)
{
}


BluetoothMessageConnectRequest::~BluetoothMessageConnectRequest()
{
}


BluetoothMessageDisconnectRequest::BluetoothMessageDisconnectRequest()
	: BluetoothMessage(BluetoothMsgId::DisconnectRequest)
{
}


BluetoothMessageDisconnectRequest::~BluetoothMessageDisconnectRequest()
{
}


BluetoothMessageTransferApduRequest::BluetoothMessageTransferApduRequest()
	: BluetoothMessage(BluetoothMsgId::TransferApduRequest)
{
}


BluetoothMessageTransferApduRequest::~BluetoothMessageTransferApduRequest()
{
}


BluetoothMessageTransferAtrRequest::BluetoothMessageTransferAtrRequest()
	: BluetoothMessage(BluetoothMsgId::TransferAtrRequest)
{
}


BluetoothMessageTransferAtrRequest::~BluetoothMessageTransferAtrRequest()
{
}


BluetoothMessagePowerSimOnRequest::BluetoothMessagePowerSimOnRequest()
	: BluetoothMessage(BluetoothMsgId::PowerSimOnRequest)
{
}


BluetoothMessagePowerSimOnRequest::~BluetoothMessagePowerSimOnRequest()
{
}


BluetoothMessagePowerSimOffRequest::BluetoothMessagePowerSimOffRequest()
	: BluetoothMessage(BluetoothMsgId::PowerSimOffRequest)
{
}


BluetoothMessagePowerSimOffRequest::~BluetoothMessagePowerSimOffRequest()
{
}


BluetoothMessageResetSimRequest::BluetoothMessageResetSimRequest()
	: BluetoothMessage(BluetoothMsgId::ResetSimRequest)
{
}


BluetoothMessageResetSimRequest::~BluetoothMessageResetSimRequest()
{
}


BluetoothMessageTransferCardReaderStatusRequest::BluetoothMessageTransferCardReaderStatusRequest()
	: BluetoothMessage(BluetoothMsgId::TransferCardReaderStatusRequest)
{
}


BluetoothMessageTransferCardReaderStatusRequest::~BluetoothMessageTransferCardReaderStatusRequest()
{
}


BluetoothMessageSetTransportProtocolRequest::BluetoothMessageSetTransportProtocolRequest()
	: BluetoothMessage(BluetoothMsgId::SetTransportProtocolRequest)
{
}


BluetoothMessageSetTransportProtocolRequest::~BluetoothMessageSetTransportProtocolRequest()
{
}
