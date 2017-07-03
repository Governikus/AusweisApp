/*!
 * BluetoothMessageCreator.h
 *
 * \brief Provides an easy to use API to create requests.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "BluetoothMessage.h"

namespace governikus
{
class BluetoothMessageConnectRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageConnectRequest();

	public:
		virtual ~BluetoothMessageConnectRequest();
};

class BluetoothMessageDisconnectRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageDisconnectRequest();

	public:
		virtual ~BluetoothMessageDisconnectRequest();
};

class BluetoothMessageTransferApduRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageTransferApduRequest();

	public:
		virtual ~BluetoothMessageTransferApduRequest();
};

class BluetoothMessageTransferAtrRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageTransferAtrRequest();

	public:
		virtual ~BluetoothMessageTransferAtrRequest();
};

class BluetoothMessagePowerSimOnRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessagePowerSimOnRequest();

	public:
		virtual ~BluetoothMessagePowerSimOnRequest();
};

class BluetoothMessagePowerSimOffRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessagePowerSimOffRequest();

	public:
		virtual ~BluetoothMessagePowerSimOffRequest();
};

class BluetoothMessageResetSimRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageResetSimRequest();

	public:
		virtual ~BluetoothMessageResetSimRequest();
};

class BluetoothMessageTransferCardReaderStatusRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageTransferCardReaderStatusRequest();

	public:
		virtual ~BluetoothMessageTransferCardReaderStatusRequest();
};

class BluetoothMessageSetTransportProtocolRequest
	: public BluetoothMessage
{
	friend class BluetoothMessageCreator;
	BluetoothMessageSetTransportProtocolRequest();

	public:
		virtual ~BluetoothMessageSetTransportProtocolRequest();
};


class BluetoothMessageCreator
{
	private:
		BluetoothMessageCreator();
		Q_DISABLE_COPY(BluetoothMessageCreator)

	public:
		static BluetoothMessageConnectRequest createConnectRequest(uint pMaxMsgSize = 20);
		static BluetoothMessageDisconnectRequest createDisconnectRequest();
		static BluetoothMessageTransferApduRequest createTransferApduRequest(const QByteArray& pApdu);
		static BluetoothMessageTransferAtrRequest createTransferAtrRequest();
		static BluetoothMessagePowerSimOnRequest createPowerSimOnRequest();
		static BluetoothMessagePowerSimOffRequest createPowerSimOffRequest();
		static BluetoothMessageResetSimRequest createResetSimRequest();
		static BluetoothMessageTransferCardReaderStatusRequest createTransferCardReaderStatusRequest();
		static BluetoothMessageSetTransportProtocolRequest createSetTransportProtocolRequest(BluetoothTransportProtocol pProtocol);
};


} /* namespace governikus */
