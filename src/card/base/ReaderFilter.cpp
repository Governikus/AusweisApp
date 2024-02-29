/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderFilter.h"

#include "ReaderConfigurationInfo.h"

#include <QMutableListIterator>

using namespace governikus;

ReaderFilter::ReaderFilter()
	: mFilterType(NoFilter)
	, mPluginTypes()
{
}


ReaderFilter::ReaderFilter(const QList<ReaderManagerPlugInType>& pPluginTypes)
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
		QMutableListIterator<ReaderInfo> iter(filtered);
		while (iter.hasNext())
		{
			const ReaderInfo entry = iter.next();
			if (!mPluginTypes.contains(entry.getPlugInType()))
			{
				iter.remove();
			}
		}
	}

	if (mFilterType & UniqueReaderTypes)
	{
		QList<ReaderConfigurationInfo> alreadyContained;

		QMutableListIterator<ReaderInfo> iter(filtered);
		while (iter.hasNext())
		{
			const ReaderConfigurationInfo configurationInfo = iter.next().getReaderConfigurationInfo();
			if (alreadyContained.contains(configurationInfo))
			{
				iter.remove();
			}
			else
			{
				alreadyContained += configurationInfo;
			}
		}
	}

	return filtered;
}
