/*!
 * \brief Parses messages of bluetooth SIM ACCESS protocol.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "BluetoothMessage.h"

namespace governikus
{

class BluetoothMessageParser
{
	private:
		QVector<QSharedPointer<const BluetoothMessage>> mMessages;
		QByteArray mRemainingBytes;

		inline ushort getParamLength(uchar pHigh, uchar pLow) const;
		void parse();
		bool parseParameter(QSharedPointer<BluetoothMessage> pMessage, int pParamCount);

		QSharedPointer<BluetoothMessage> createMessage(BluetoothMsgId pMsgId);
		QSharedPointer<BluetoothMessageParameter> createMessageParameter(BluetoothParamId pParamId, const QByteArray& pValue);

	public:
		BluetoothMessageParser(const QByteArray& pData);
		virtual ~BluetoothMessageParser();

		const QVector<QSharedPointer<const BluetoothMessage>>& getMessages() const;
		const QByteArray& getRemainingBytes() const;
};

} // namespace governikus
