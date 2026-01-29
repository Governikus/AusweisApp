/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdList.h"

#include <QTimer>


namespace governikus
{

class IfdListImpl
	: public IfdList
{
	Q_OBJECT

	private:
		QTimer mTimer;
		const int mReaderResponsiveTimeout;
		QList<QSharedPointer<IfdListEntry>> mResponsiveList;

	private Q_SLOTS:
		void onProcessUnresponsiveRemoteReaders();

	public:
		explicit IfdListImpl(int pCheckInterval = 1000, int pReaderResponsiveTimeout = 5000);
		~IfdListImpl() override;

		void update(const Discovery& pDiscovery) override;
		void clear() override;
		[[nodiscard]] QList<QSharedPointer<IfdListEntry>> getIfdList() const override;
};

} // namespace governikus
