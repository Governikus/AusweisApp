/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
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
		QByteArray mId;
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
				const QByteArray& pId,
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
		[[nodiscard]] const QByteArray& getId() const;
		void setId(const QByteArray& pId);
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
