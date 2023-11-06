/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
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


ReaderFilter::ReaderFilter(const QVector<ReaderManagerPlugInType>& pPluginTypes)
	: mFilterType(PluginTypeFilter)
	, mPluginTypes(pPluginTypes)
{
}


ReaderFilter::ReaderFilter(const ReaderFilter::FilterType pFilterType)
	: mFilterType(pFilterType)
	, mPluginTypes()
{
}


QVector<ReaderInfo> ReaderFilter::apply(const QVector<ReaderInfo>& pInputList) const
{
	QVector<ReaderInfo> filtered = pInputList;

	if (mFilterType & PluginTypeFilter)
	{
		QMutableVectorIterator<ReaderInfo> iter(filtered);
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
		QVector<ReaderConfigurationInfo> alreadyContained;

		QMutableVectorIterator<ReaderInfo> iter(filtered);
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
