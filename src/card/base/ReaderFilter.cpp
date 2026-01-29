/**
 * Copyright (c) 2017-2026 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderFilter.h"

#include "ReaderConfigurationInfo.h"

using namespace governikus;

ReaderFilter::ReaderFilter()
	: mFilterType(NoFilter)
	, mPluginTypes()
{
}


ReaderFilter::ReaderFilter(const QList<ReaderManagerPluginType>& pPluginTypes)
	: mFilterType(PluginTypeFilter)
	, mPluginTypes(pPluginTypes)
{
}


ReaderFilter::ReaderFilter(const ReaderFilter::FilterType pFilterType)
	: mFilterType(pFilterType)
	, mPluginTypes()
{
}


QList<ReaderInfo> ReaderFilter::apply(const QList<ReaderInfo>& pInputList) const
{
	QList<ReaderInfo> filtered = pInputList;

	if (mFilterType & PluginTypeFilter)
	{
		erase_if(filtered, [this](const ReaderInfo& pEntry) {
					return !mPluginTypes.contains(pEntry.getPluginType());
				});
	}

	if (mFilterType & UniqueReaderTypes)
	{
		QList<ReaderConfigurationInfo> alreadyContained;

		erase_if(filtered, [&alreadyContained](const ReaderInfo& pEntry) {
					const auto configurationInfo = pEntry.getReaderConfigurationInfo();
					if (alreadyContained.contains(configurationInfo))
					{
						return true;
					}
					alreadyContained += configurationInfo;
					return false;
				});
	}

	return filtered;
}
