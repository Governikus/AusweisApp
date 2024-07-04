/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
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

defineEnumTypeQmlExposed(ReaderManagerPluginType
		, UNKNOWN
		, MOCK
		, PCSC
		, NFC
		, REMOTE_IFD
		, LOCAL_IFD
		, SMART
		, SIMULATOR
		)


class ReaderManagerPluginInfo
{
	public:
		enum class Key
		{
			PCSC_LITE_VERSION, LOCAL_IFD_STATE
		};

		enum class InitialScan
		{
			SUCCEEDED,
			FAILED,
			UNKNOWN
		};

	public:
		ReaderManagerPluginInfo(ReaderManagerPluginType pType = ReaderManagerPluginType::UNKNOWN,
				bool pEnabled = false,
				bool pAvailable = false);

		[[nodiscard]] const ReaderManagerPluginType& getPluginType() const
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


		[[nodiscard]] InitialScan getInitialScanState() const
		{
			return mInitialScan;
		}


		void setInitialScanState(InitialScan pInitialScan)
		{
			mInitialScan = pInitialScan;
		}

	private:
		ReaderManagerPluginType mType;
		QMap<Key, QVariant> mValues;
		bool mEnabled;
		bool mAvailable;
		bool mScanRunning;
		InitialScan mInitialScan;

};

} // namespace governikus
