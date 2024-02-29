/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Model implementation for the remote device table
 */

#pragma once

#include "IfdListEntry.h"

#include <QDateTime>
#include <QSharedPointer>
#include <QString>
#include <QTimer>


class test_RemoteDeviceModel;


namespace governikus
{

class RemoteDeviceModelEntry
{
	friend class ::test_RemoteDeviceModel;

	private:
		QString mDeviceName;
		QString mId;
		bool mPaired;
		bool mIsPairing;
		bool mNetworkVisible;
		bool mConnected;
		bool mSupported;
		QDateTime mLastConnected;
		QSharedPointer<IfdListEntry> mRemoteDeviceListEntry;

	public:
		explicit RemoteDeviceModelEntry(const QSharedPointer<IfdListEntry>& pListEntry);
		RemoteDeviceModelEntry(const QString& pDeviceNameEscaped,
				const QString& mId,
				bool pNetworkVisible,
				bool pConnected,
				bool pSupported,
				bool pIsPairing,
				const QDateTime& pLastConnected,
				const QSharedPointer<IfdListEntry>& pRemoteDeviceListEntry);
		explicit RemoteDeviceModelEntry(const QString& pDeviceNameEscaped = QStringLiteral("UnknownReader"));

		[[nodiscard]] bool isPaired() const;
		void setPaired(bool pPaired);
		[[nodiscard]] bool isPairing() const;
		void setIsPairing(bool pIsPairing);
		[[nodiscard]] const QString& getId() const;
		void setId(const QString& pId);
		[[nodiscard]] bool isNetworkVisible() const;
		[[nodiscard]] int getLinkQuality() const;
		[[nodiscard]] bool isSupported() const;
		void setNetworkVisible(bool pNetworkVisible);
		[[nodiscard]] const QDateTime& getLastConnected() const;
		void setLastConnected(const QDateTime& pLastConnected);
		[[nodiscard]] bool operator==(const RemoteDeviceModelEntry& pOther) const;

		[[nodiscard]] const QSharedPointer<IfdListEntry>& getRemoteDeviceListEntry() const;
		[[nodiscard]] QString getDeviceNameEscaped() const;
};

} // namespace governikus
