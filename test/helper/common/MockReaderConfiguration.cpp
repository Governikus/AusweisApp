/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "MockReaderConfiguration.h"

using namespace governikus;


void MockReaderConfiguration::clearReaderConfiguration()
{
	mReaderConfigurationInfos.clear();
}


QList<ReaderConfigurationInfo>& MockReaderConfiguration::readerConfigurationInfos()
{
	return mReaderConfigurationInfos;
}
