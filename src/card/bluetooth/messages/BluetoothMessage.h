/*!
 * BluetoothMessage.h
 *
 * \brief Implements a message of SIM ACCESS profile.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "parameter/BluetoothMessageParameter.h"
#include <QSharedPointer>
#include <QVector>

namespace governikus
{

class BluetoothMessage
{
	public:
		typedef QSharedPointer<const BluetoothMessage> Ptr;

	private:
		typedef QVector<BluetoothMessageParameter::Ptr> ParameterList;

		BluetoothMsgId mMsgId;
		ParameterList mMessageParameter;

	protected:
		template<typename T> QSharedPointer<const T> getParameter() const
		{
			for (const auto& param : getParameterList())
			{
				const auto t = param.dynamicCast<const T>();
				if (t)
				{
					return t;
				}
			}
			return QSharedPointer<const T>();
		}


	public:
		BluetoothMessage(BluetoothMsgId pMsgId);
		virtual ~BluetoothMessage();

		void addParameter(BluetoothMessageParameter::Ptr pMessageParameter);
		template<typename T> void copyParameter(const T& pMessageParameter)
		{
			addParameter(BluetoothMessageParameter::Ptr(new T(pMessageParameter)));
		}


		const ParameterList& getParameterList() const;
		BluetoothMsgId getBluetoothMsgId() const;
		QByteArray toData() const;
		QString toString() const;

		template<typename T> const T* get() const
		{
			return static_cast<const T*>(this);
		}


};

} /* namespace governikus */


QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessage& pMsg);
