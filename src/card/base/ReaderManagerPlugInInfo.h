/*!
 * ReaderManagerPlugInInfo.h
 *
 * \brief Data object providing information about a reader manager plug-in.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "EnumHelper.h"
#include "MetaTypeHelper.h"

#include <QMap>
#include <QString>
#include <QVariant>


namespace governikus
{


defineEnumType(ReaderManagerPlugInType, UNKNOWN, PCSC, BLUETOOTH, NFC)


class ReaderManagerPlugInInfo
{
	public:
		enum class Key
		{
			PCSC_LITE_VERSION,
		};

	public:
		ReaderManagerPlugInInfo(ReaderManagerPlugInType pType = ReaderManagerPlugInType::UNKNOWN, bool pEnabled = false);


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


	private:
		ReaderManagerPlugInType mType;
		QMap<Key, QVariant> mValues;
		bool mEnabled;
};

} /* namespace governikus */

REGISTER_META_TYPE(ReaderManagerPlugInInfo)
