/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Interface for IfdList
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
		IfdListImpl(int pCheckInterval = 1000, int pReaderResponsiveTimeout = 5000);
		~IfdListImpl() override;

		void update(const IfdDescriptor& pDescriptor) override;
		void clear() override;
		[[nodiscard]] QList<QSharedPointer<IfdListEntry>> getIfdList() const override;
};

} // namespace governikus
