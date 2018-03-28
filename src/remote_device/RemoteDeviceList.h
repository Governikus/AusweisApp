/*!
 * \brief Interface for RemoteDeviceList
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
		const RemoteDeviceDescriptor mRemoteDeviceDescriptor;
		QTime mLastSeen;

	public:
		RemoteDeviceListEntry(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor);

		void setLastSeenToNow();

		bool contains(const RemoteDeviceDescriptor& pRemoteDeviceDescriptor) const;
		bool isEqual(const RemoteDeviceListEntry* const pOther) const;

		const QTime& getLastSeen() const;
		const RemoteDeviceDescriptor& getRemoteDeviceDescriptor() const;

};


class RemoteDeviceList
	: public QObject
{
	Q_OBJECT

	Q_SIGNALS:
		void fireDeviceAppeared(const QSharedPointer<RemoteDeviceListEntry>&);
		void fireDeviceVanished(const QSharedPointer<RemoteDeviceListEntry>&);

	public:
		RemoteDeviceList(int pCheckInterval, int pTimeout);
		virtual ~RemoteDeviceList();

		virtual void update(const RemoteDeviceDescriptor& pDescriptor) = 0;
		virtual void clear() = 0;
		virtual QVector<QSharedPointer<RemoteDeviceListEntry> > getRemoteDevices() const;
};


class RemoteDeviceListImpl
	: public RemoteDeviceList
{
	Q_OBJECT

	private:
		QTimer mTimer;
		const int mTimeout;
		QVector<QSharedPointer<RemoteDeviceListEntry> > mList;

	private Q_SLOTS:
		void onRemoveUnresponsiveRemoteReaders();

	public:
		RemoteDeviceListImpl(int pCheckInterval = 1000, int pTimeout = 5000);
		virtual ~RemoteDeviceListImpl() override;

		virtual void update(const RemoteDeviceDescriptor& pDescriptor) override;
		virtual void clear() override;
		virtual QVector<QSharedPointer<RemoteDeviceListEntry> > getRemoteDevices() const override;
};


} /* namespace governikus */
