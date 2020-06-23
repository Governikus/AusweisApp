/*!
 * \brief Data object providing information about a reader manager plug-in.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QMap>
#include <QString>
#include <QVariant>

namespace governikus
{

defineEnumType(ReaderManagerPlugInType, UNKNOWN, PCSC, BLUETOOTH, NFC, REMOTE)


class ReaderManagerPlugInInfo
{
	public:
		enum class Key
		{
			PCSC_LITE_VERSION,
		};

	public:
		ReaderManagerPlugInInfo(ReaderManagerPlugInType pType = ReaderManagerPlugInType::UNKNOWN,
				bool pEnabled = false,
				bool pAvailable = false);

		const ReaderManagerPlugInType& getPlugInType() const
		{
			return mType;
		}


		bool hasValue(Key pKey)
		{
			return mValues.contains(pKey);
		}


		QVariant getValue(Key pKey) const
		{
			return mValues.value(pKey);
		}


		void setValue(Key pKey, const QVariant& pValue)
		{
			mValues.insert(pKey, pValue);
		}


		/*!
		 * Is the interface / daemon process etc. enabled?
		 */
		bool isEnabled() const
		{
			return mEnabled;
		}


		void setEnabled(bool pEnabled)
		{
			mEnabled = pEnabled;
		}


		/*!
		 * Is there a device/interface?
		 */
		bool isAvailable() const
		{
			return mAvailable;
		}


		void setAvailable(bool pAvailable)
		{
			mAvailable = pAvailable;
		}


		bool isResponding() const
		{
			return mResponding;
		}


		void setResponding(bool pResponding)
		{
			mResponding = pResponding;
		}

	private:
		ReaderManagerPlugInType mType;
		QMap<Key, QVariant> mValues;
		bool mEnabled;
		bool mAvailable;
		bool mResponding;
};

} // namespace governikus
