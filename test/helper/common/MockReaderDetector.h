/*!
 * \brief ReaderDetector mock for tests
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
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
	virtual QVector<UsbId> attachedDevIds() const;
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
		MockReaderDetector(const QVector<UsbId>& pDevIds);

		~MockReaderDetector() override;

		[[nodiscard]] QVector<UsbId> attachedDevIds() const override;

	private:
		const QVector<UsbId>& mDevIds;
};

} // namespace governikus
