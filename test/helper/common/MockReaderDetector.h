/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "UsbId.h"

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	#include <QObject>

namespace governikus
{
class ReaderDetector
	: public QObject
{
	virtual QList<UsbId> attachedDevIds() const;
};
} // namespace governikus
#else
	#include "ReaderDetector.h"
#endif

namespace governikus
{

class MockReaderDetector
	: public ReaderDetector
{
	Q_OBJECT

	public:
		MockReaderDetector(const QList<UsbId>& pDevIds);

		~MockReaderDetector() override;

		[[nodiscard]] QList<UsbId> attachedDevIds() const override;

	private:
		const QList<UsbId>& mDevIds;
};

} // namespace governikus
