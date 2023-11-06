/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Data object providing information about a reader manager plug-in.
 */

#pragma once

#include "EnumHelper.h"

#include <QMap>
#include <QString>
#include <QVariant>

namespace governikus
{

defineEnumType(ReaderManagerPlugInType
		, UNKNOWN
		, MOCK
		, PCSC
		, NFC
		, REMOTE_IFD
		, LOCAL_IFD
		, SMART
		, SIMULATOR
		)


class ReaderManagerPlugInInfo
{
	public:
		enum class Key
		{
			PCSC_LITE_VERSION, LOCAL_IFD_STATE
		};

	public:
		ReaderManagerPlugInInfo(ReaderManagerPlugInType pType = ReaderManagerPlugInType::UNKNOWN,
				bool pEnabled = false,
				bool pAvailable = false);

		[[nodiscard]] const ReaderManagerPlugInType& getPlugInType() const
		{
			return mType;
		}


		[[nodiscard]] bool hasValue(Key pKey) const
		{
			return mValues.contains(pKey);
		}


		[[nodiscard]] QVariant getValue(Key pKey) const
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
		[[nodiscard]] bool isEnabled() const
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
		[[nodiscard]] bool isAvailable() const
		{
			return mAvailable;
		}


		void setAvailable(bool pAvailable)
		{
			mAvailable = pAvailable;
		}


		[[nodiscard]] bool isScanRunning() const
		{
			return mScanRunning;
		}


		void setScanRunning(bool pScanRunning)
		{
			mScanRunning = pScanRunning;
		}

	private:
		ReaderManagerPlugInType mType;
		QMap<Key, QVariant> mValues;
		bool mEnabled;
		bool mAvailable;
		bool mScanRunning;


};

} // namespace governikus
