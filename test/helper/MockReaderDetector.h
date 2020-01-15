/*!
 * \brief ReaderDetector mock for tests
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "ReaderDetector.h"


namespace governikus
{

class MockReaderDetector
	: public ReaderDetector
{
	Q_OBJECT

	public:
		MockReaderDetector(const QVector<UsbId>& pDevIds);

		virtual ~MockReaderDetector() override;

		QVector<UsbId> attachedDevIds() const override;

	private:
		const QVector<UsbId>& mDevIds;
};

} // namespace governikus
