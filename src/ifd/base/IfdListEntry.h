/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/Discovery.h"

#include <QList>
#include <QTime>


namespace governikus
{

class IfdListEntry
{
	Q_DISABLE_COPY(IfdListEntry)

	private:
		Discovery mDiscovery;
		QTime mLastSeen;
		QList<QTime> mLastSeenHistory;

	public:
		explicit IfdListEntry(const Discovery& pDiscovery);

		void setLastSeenToNow();
		bool cleanUpSeenTimestamps(int pReaderResponsiveTimeout);
		[[nodiscard]] int getPercentSeen(int pCheckInterval = 1000, int pTimeFrame = 5000) const;

		void setDiscovery(const Discovery& pDiscovery);

		[[nodiscard]] bool containsIfdId(const QByteArray& pIfdId) const;

		[[nodiscard]] const QTime& getLastSeen() const;
		[[nodiscard]] const Discovery& getDiscovery() const;

};

} // namespace governikus
