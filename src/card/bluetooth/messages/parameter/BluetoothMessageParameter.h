/*!
 * \brief Implements message parameter of SIM ACCESS spec.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/BluetoothIDs.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(bluetooth)


namespace governikus
{

class BluetoothMessageParameter
{
	private:
		BluetoothParamId mParamId;

	protected:
		QByteArray mValue;
		bool mValid;

		inline QString toStringBase() const
		{
			return mParamId + QStringLiteral(" | Value: ");
		}


		template<typename T> bool parseByteParameter(const QByteArray& pValue, T& pDest) const
		{
			if (pValue.size() == 1)
			{
				pDest = static_cast<T>(pValue.at(0));
				if (!Enum<T>::isValue(pValue.at(0)))
				{
					qCWarning(bluetooth) << "Value is unknown:" << pDest;
				}
				return true;
			}
			else
			{
				qCWarning(bluetooth) << "Content has wrong size:" << pValue.toHex();
			}

			return false;
		}


	public:
		BluetoothMessageParameter(BluetoothParamId pParamId, const QByteArray& pValue);
		virtual ~BluetoothMessageParameter();

		BluetoothParamId getParameterId() const;
		const QByteArray& getValue() const;
		QByteArray toData() const;
		bool isValid() const;

		virtual QString toString() const;
		virtual QString toStringValue() const;
};

} // namespace governikus

QDebug operator<<(QDebug pDbg, const governikus::BluetoothMessageParameter& pMsg);
