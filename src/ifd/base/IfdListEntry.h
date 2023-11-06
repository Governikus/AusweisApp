/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Interface for IfdList
 */

#pragma once

#include "IfdDescriptor.h"

#include <QTime>
#include <QVector>


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

} // namespace governikus
