/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief ReaderConfigurationFile mock for tests
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
