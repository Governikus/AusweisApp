/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "BluetoothMessageConnectResponse.h"
#include "BluetoothMessageDisconnectResponse.h"
#include "BluetoothMessageParser.h"
#include "BluetoothMessagePowerSimOffResponse.h"
#include "BluetoothMessagePowerSimOnResponse.h"
#include "BluetoothMessageResetSimResponse.h"
#include "BluetoothMessageSetTransportProtocolResponse.h"
#include "BluetoothMessageStatusInd.h"
#include "BluetoothMessageTransferApduResponse.h"
#include "BluetoothMessageTransferCardReaderStatusResponse.h"
#include "BluetoothUtils.h"
#include "parameter/BluetoothMessageParameterApduResponse.h"
#include "parameter/BluetoothMessageParameterCardReaderStatus.h"
#include "parameter/BluetoothMessageParameterConnectionStatus.h"
#include "parameter/BluetoothMessageParameterMaxMsgSize.h"
#include "parameter/BluetoothMessageParameterResultCode.h"
#include "parameter/BluetoothMessageParameterStatusChange.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card)


BluetoothMessageParser::BluetoothMessageParser(const QByteArray& pData)
	: mMessages()
	, mRemainingBytes(pData)
{
	parse();
}


BluetoothMessageParser::~BluetoothMessageParser()
{

}


void BluetoothMessageParser::parse()
{
	while (mRemainingBytes.size() >= 4)
	{
		BluetoothMsgId msgId = static_cast<BluetoothMsgId>(mRemainingBytes.at(0));
		int parameterCount = mRemainingBytes.at(1);
		auto message = createMessage(msgId);

		if (parseParameter(message, parameterCount))
		{
			mMessages += message;
		}
		else
		{
			break;
		}
	}
}


bool BluetoothMessageParser::parseParameter(QSharedPointer<BluetoothMessage> pMessage, int pParamCount)
{
	QByteArray data = mRemainingBytes.mid(4); // the message header is parsed by parse(QByteArray)

	for (int i = 0; i < pParamCount; ++i)
	{
		if (data.size() >= 4)
		{
			BluetoothParamId paramId = static_cast<BluetoothParamId>(data.at(0));
			ushort paramLength = getParamLength(static_cast<uchar>(data.at(2)), static_cast<uchar>(data.at(3)));
			data = data.mid(4);

			/*!
			 * According to the SAP specification, see chapter 5.1
			 * "The length of each Parameter shall be a multiple of four bytes. Therefore, one to three
			 *   additional bytes have to be added directly after the "Parameter Value""
			 */
			if (data.size() >= paramLength + BluetoothUtils::getPaddingLength(paramLength))
			{
				QByteArray value = data.mid(0, paramLength);
				auto param = createMessageParameter(paramId, value);
				pMessage->addParameter(param);
				data = data.mid(paramLength + BluetoothUtils::getPaddingLength(paramLength));
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	mRemainingBytes = data;
	return true;
}


ushort BluetoothMessageParser::getParamLength(uchar pHigh, uchar pLow) const
{
	return static_cast<ushort>((pHigh << 8) + pLow);
}


const QVector<BluetoothMessage::Ptr>& BluetoothMessageParser::getMessages() const
{
	return mMessages;
}


const QByteArray& BluetoothMessageParser::getRemainingBytes() const
{
	return mRemainingBytes;
}


QSharedPointer<BluetoothMessage> BluetoothMessageParser::createMessage(BluetoothMsgId pMsgId)
{
	BluetoothMessage* msg = nullptr;

	switch (pMsgId)
	{
		case BluetoothMsgId::ConnectResponse:
			msg = new BluetoothMessageConnectResponse();
			break;

		case BluetoothMsgId::DisconnectResponse:
			msg = new BluetoothMessageDisconnectResponse();
			break;

		case BluetoothMsgId::StatusInd:
			msg = new BluetoothMessageStatusInd();
			break;

		case BluetoothMsgId::TransferCardReaderStatusResponse:
			msg = new BluetoothMessageTransferCardReaderStatusResponse();
			break;

		case BluetoothMsgId::TransferApduResponse:
			msg = new BluetoothMessageTransferApduResponse();
			break;

		case BluetoothMsgId::PowerSimOffResponse:
			msg = new BluetoothMessagePowerSimOffResponse();
			break;

		case BluetoothMsgId::PowerSimOnResponse:
			msg = new BluetoothMessagePowerSimOnResponse();
			break;

		case BluetoothMsgId::ResetSimResponse:
			msg = new BluetoothMessageResetSimResponse();
			break;

		case BluetoothMsgId::SetTransportProtocolResponse:
			msg = new BluetoothMessageSetTransportProtocolResponse();
			break;

		default:
			msg = new BluetoothMessage(pMsgId);
	}

	Q_ASSERT(msg != nullptr);
	return QSharedPointer<BluetoothMessage>(msg);
}


QSharedPointer<BluetoothMessageParameter> BluetoothMessageParser::createMessageParameter(BluetoothParamId pParamId,
		const QByteArray& pValue)
{
	BluetoothMessageParameter* param = nullptr;

	switch (pParamId)
	{
		case BluetoothParamId::StatusChange:
			param = new BluetoothMessageParameterStatusChange(pValue);
			break;

		case BluetoothParamId::ResultCode:
			param = new BluetoothMessageParameterResultCode(pValue);
			break;

		case BluetoothParamId::ConnectionStatus:
			param = new BluetoothMessageParameterConnectionStatus(pValue);
			break;

		case BluetoothParamId::ResponseAPDU:
			param = new BluetoothMessageParameterApduResponse(pValue);
			break;

		case BluetoothParamId::MaxMsgSize:
			param = new BluetoothMessageParameterMaxMsgSize(pValue);
			break;

		case BluetoothParamId::CardReaderStatus:
			param = new BluetoothMessageParameterCardReaderStatus(pValue);
			break;

		default:
			param = new BluetoothMessageParameter(pParamId, pValue);
	}

	Q_ASSERT(param != nullptr);
	return QSharedPointer<BluetoothMessageParameter>(param);
}
