/*!
 * \brief Implements a message of SIM ACCESS profile.
 *
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
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
	private:
		friend class ::test_BluetoothMessageParser;
		BluetoothMsgId mMsgId;
		QMap<BluetoothParamId, QSharedPointer<const BluetoothMessageParameter> > mMessageParameter;

	protected:
		QSharedPointer<const BluetoothMessageParameter> getParameter(BluetoothParamId pId) const;

	public:
		BluetoothMessage(BluetoothMsgId pMsgId);
		virtual ~BluetoothMessage();

		void addParameter(const QSharedPointer<const BluetoothMessageParameter>& pMessageParameter);
		template<typename T> void copyParameter(const T& pMessageParameter)
		{
			addParameter(QSharedPointer<T>::create(pMessageParameter));
		}


		BluetoothMsgId getBluetoothMsgId() const;
		QByteArray toData() const;
		QString toString() const;
};

} // namespace governikus


QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessage& pMsg);
