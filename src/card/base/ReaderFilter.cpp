/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#include "ReaderFilter.h"


using namespace governikus;


ReaderFilter::ReaderFilter()
	: mfilter(false)
	, mPluginTypes()
{
}


ReaderFilter::ReaderFilter(const QList<ReaderManagerPluginType>& pPluginTypes)
	: mfilter(true)
	, mPluginTypes(pPluginTypes)
{
}


QList<ReaderInfo> ReaderFilter::apply(const QList<ReaderInfo>& pInputList) const
{
	if (!mfilter)
	{
		return pInputList;
	}

	QList<ReaderInfo> filtered = pInputList;
	erase_if(filtered, [this](const ReaderInfo& pEntry) {
				return !mPluginTypes.contains(pEntry.getPluginType());
			});
	return filtered;
}
