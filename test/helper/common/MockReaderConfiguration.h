/*!
 * \brief ReaderConfigurationFile mock for tests
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "ReaderConfiguration.h"

namespace governikus
{

class MockReaderConfiguration
	: public ReaderConfiguration
{
	Q_OBJECT

	public:
		MockReaderConfiguration() = default;
		~MockReaderConfiguration() override = default;

		void clearReaderConfiguration();
};

} // namespace governikus
