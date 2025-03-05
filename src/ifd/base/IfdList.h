/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdDescriptor.h"
#include "IfdListEntry.h"

#include <QList>


namespace governikus
{

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
		[[nodiscard]] virtual QList<QSharedPointer<IfdListEntry>> getIfdList() const;
};

} // namespace governikus
