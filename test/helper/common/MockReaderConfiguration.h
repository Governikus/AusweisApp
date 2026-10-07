/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
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

		QList<ReaderConfigurationInfo>& readerConfigurationInfos();
};

} // namespace governikus
