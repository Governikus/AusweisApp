/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Interface for IfdList
 */

#pragma once

#include "IfdDescriptor.h"

#include <QTime>
#include <QTimer>


namespace governikus
{

class IfdListEntry
{
	Q_DISABLE_COPY(IfdListEntry)

	private:
		IfdDescriptor mIfdDescriptor;
		QTime mLastSeen;
		QVector<QTime> mLastSeenHistory;

	public:
		explicit IfdListEntry(const IfdDescriptor& pIfdDescriptor);

		void setLastSeenToNow();
		bool cleanUpSeenTimestamps(int pReaderResponsiveTimeout);
		[[nodiscard]] int getPercentSeen(int pCheckInterval = 1000, int pTimeFrame = 5000) const;

		void setIfdDescriptor(const IfdDescriptor& pIfdDescriptor);

		[[nodiscard]] bool containsEquivalent(const IfdDescriptor& pIfdDescriptor) const;
		bool isEqual(const IfdListEntry* const pOther) const;

		[[nodiscard]] const QTime& getLastSeen() const;
		[[nodiscard]] const IfdDescriptor& getIfdDescriptor() const;

};


class IfdList
	: public QObject
{
	Q_OBJECT

	Q_SIGNALS:
		void fireDeviceAppeared(const QSharedPointer<IfdListEntry>&);
		void fireDeviceUpdated(const QSharedPointer<IfdListEntry>&);
		void fireDeviceVanished(const QSharedPointer<IfdListEntry>&);

	public:
		IfdList() = default;
		~IfdList() override = default;

		virtual void update(const IfdDescriptor& pDescriptor) = 0;
		virtual void clear() = 0;
		[[nodiscard]] virtual QVector<QSharedPointer<IfdListEntry>> getIfdList() const;
};


class IfdListImpl
	: public IfdList
{
	Q_OBJECT

	private:
		QTimer mTimer;
		const int mReaderResponsiveTimeout;
		QVector<QSharedPointer<IfdListEntry>> mResponsiveList;

	private Q_SLOTS:
		void onProcessUnresponsiveRemoteReaders();

	public:
		IfdListImpl(int pCheckInterval = 1000, int pReaderResponsiveTimeout = 5000);
		~IfdListImpl() override;

		void update(const IfdDescriptor& pDescriptor) override;
		void clear() override;
		[[nodiscard]] QVector<QSharedPointer<IfdListEntry>> getIfdList() const override;
};


} // namespace governikus
