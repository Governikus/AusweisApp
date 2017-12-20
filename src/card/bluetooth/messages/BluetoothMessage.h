/*!
 * \brief Implements a message of SIM ACCESS profile.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "parameter/BluetoothMessageParameter.h"

#include <QSharedPointer>
#include <QVector>

class test_BluetoothMessageParser;

namespace governikus
{

class BluetoothMessage
{
	public:
		using Ptr = QSharedPointer<const BluetoothMessage>;

	private:
		friend class ::test_BluetoothMessageParser;
		BluetoothMsgId mMsgId;
		QMap<BluetoothParamId, BluetoothMessageParameter::Ptr> mMessageParameter;

	protected:
		BluetoothMessageParameter::Ptr getParameter(BluetoothParamId pId) const;

	public:
		BluetoothMessage(BluetoothMsgId pMsgId);
		virtual ~BluetoothMessage();

		void addParameter(BluetoothMessageParameter::Ptr pMessageParameter);
		template<typename T> void copyParameter(const T& pMessageParameter)
		{
			addParameter(BluetoothMessageParameter::Ptr(new T(pMessageParameter)));
		}


		BluetoothMsgId getBluetoothMsgId() const;
		QByteArray toData() const;
		QString toString() const;
};

} /* namespace governikus */


QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessage& pMsg);
