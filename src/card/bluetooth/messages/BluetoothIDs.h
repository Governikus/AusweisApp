/*!
 * \brief Add message and parameter types of bluetooth SIM ACCESS spec
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

namespace governikus
{

defineTypedEnumType(BluetoothMsgId, char,
		ConnectRequest = 0x00,
		ConnectResponse = 0x01,
		DisconnectRequest = 0x02,
		DisconnectResponse = 0x03,
		DisconnectInd = 0x04,
		TransferApduRequest = 0x05,
		TransferApduResponse = 0x06,
		TransferAtrRequest = 0x07,
		TransferAtrResponse = 0x08,
		PowerSimOffRequest = 0x0A,
		PowerSimOffResponse = 0x0A,
		PowerSimOnRequest = 0x0B,
		PowerSimOnResponse = 0x0C,
		ResetSimRequest = 0x0D,
		ResetSimResponse = 0x0E,
		TransferCardReaderStatusRequest = 0x0F,
		TransferCardReaderStatusResponse = 0x10,
		StatusInd = 0x11,
		ErrorResponse = 0x12,
		SetTransportProtocolRequest = 0x13,
		SetTransportProtocolResponse = 0x14)

defineTypedEnumType(BluetoothParamId, char,
		MaxMsgSize = 0x00,
		ConnectionStatus = 0x01,
		ResultCode = 0x02,
		DisconnectionType = 0x03,
		CommandAPDU = 0x04,
		ResponseAPDU = 0x05,
		ATR = 0x06,
		CardReaderStatus = 0x07,
		StatusChange = 0x08,
		TransportProtocol = 0x09,
		CommandAPDU7816 = 0x10)

defineEnumType(BluetoothConnectionStatus,
		OkServerCanFulfillRequirements = 0x00,
		ErrorServerUnableToEstablishConnection = 0x01,
		ErrorServerDoesNotSupportMaximumMessageSize = 0x02,
		ErrorMaximumMessageSizeByClientIsTooSmall = 0x03,
		OkOngoingCall = 0x04)

defineEnumType(BluetoothDisconnectionType, Graceful = 0x00, Immediate = 0x01)

defineTypedEnumType(BluetoothResultCode, char,
		Ok = 0x00,
		ErrorNoReasonDefined = 0x01,
		ErrorCardNotAccessible = 0x02,
		ErrorCardAlreadyPoweredOff = 0x03,
		ErrorCardRemoved = 0x04,
		ErrorCardAlreadyPoweredOn = 0x05,
		ErrorDataNotAvailabe = 0x06,
		ErrorNotSupport = 0x07)

defineTypedEnumType(BluetoothStatusChange, char,
		Unknown = 0x00,
		CardReset = 0x01,
		CardNotAccessible = 0x02,
		CardRemoved = 0x03,
		CardInserted = 0x04,
		CardRecovered = 0x05)

defineTypedEnumType(BluetoothTransportProtocol, char, T0 = 0x00, T1 = 0x01)

defineEnumType(BluetoothCardReaderStatus, Unknown = 0x00, CardInserted = 0x78, CardRemoved = 0x38)

} // namespace governikus
