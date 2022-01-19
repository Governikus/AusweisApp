/*!
 * \brief Interface for RemoteDeviceList
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteDeviceDescriptor.h"

#include <QTime>
#include <QTimer>


namespace governikus
{

class RemoteDeviceListEntry
{
	Q_DISABLE_COPY(RemoteDeviceListEntry)

	private:
		RemoteDeviceDescriptor mRemoteDeviceDescriptor;
		QTime mLastSeen;
		QVector<QTime> mLastSeenHistory;

	public:
		explicit RemoteDeviceListEntry(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

		void setLastSeenToNow();
		bool cleanUpSeenTimestamps(int pReaderResponsiveTimeout);
		[[nodiscard]] int getPercentSeen(int pCheckInterval = 1000, int pTimeFrame = 5000) const;

		void setRemoteDeviceDescriptor(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

		[[nodiscard]] bool containsEquivalent(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor) const;
		bool isEqual(const RemoteDeviceListEntry* const pOther) const;

		[[nodiscard]] const QTime& getLastSeen() const;
		[[nodiscard]] const RemoteDeviceDescriptor& getRemoteDeviceDescriptor() const;

};


class RemoteDeviceList
	: public QObject
{
	Q_OBJECT

	Q_SIGNALS:
		void fireDeviceAppeared(const QSharedPointer<RemoteDeviceListEntry>&);
		void fireDeviceUpdated(const QSharedPointer<RemoteDeviceListEntry>&);
		void fireDeviceVanished(const QSharedPointer<RemoteDeviceListEntry>&);

	public:
		RemoteDeviceList(int pCheckInterval, int pTimeout);
		~RemoteDeviceList() override;

		virtual void update(const RemoteDeviceDescriptor& pDescriptor) = 0;
		virtual void clear() = 0;
		[[nodiscard]] virtual QVector<QSharedPointer<RemoteDeviceListEntry>> getRemoteDevices() const;
};


class RemoteDeviceListImpl
	: public RemoteDeviceList
{
	Q_OBJECT

	private:
		QTimer mTimer;
		const int mReaderResponsiveTimeout;
		QVector<QSharedPointer<RemoteDeviceListEntry>> mResponsiveList;

	private Q_SLOTS:
		void onProcessUnresponsiveRemoteReaders();

	public:
		RemoteDeviceListImpl(int pCheckInterval = 1000, int pReaderResponsiveTimeout = 5000);
		~RemoteDeviceListImpl() override;

		void update(const RemoteDeviceDescriptor& pDescriptor) override;
		void clear() override;
		[[nodiscard]] QVector<QSharedPointer<RemoteDeviceListEntry>> getRemoteDevices() const override;
};


} // namespace governikus
