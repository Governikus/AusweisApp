/*!
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderFilter.h"

#include "ReaderConfiguration.h"
#include "ReaderManagerPlugIn.h"

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


QVector<ReaderManagerPlugIn*> ReaderFilter::apply(const QVector<ReaderManagerPlugIn*>& pPluginType) const
{
	if (mFilterType & PluginTypeFilter)
	{
		QVector<ReaderManagerPlugIn*> filtered;
		for (const auto& pluginType : pPluginType)
		{
			if (mPluginTypes.contains(pluginType->getInfo().getPlugInType()))
			{
				filtered += pluginType;
			}
		}
		return filtered;
	}

	return pPluginType;
}


QVector<ReaderInfo> ReaderFilter::apply(const QVector<ReaderInfo>& pInputList) const
{
	if (mFilterType & UniqueReaderTypes)
	{
		QVector<ReaderInfo> filtered;
		QVector<ReaderConfigurationInfo> alreadyContained;
		for (const auto& readerInfo : pInputList)
		{
			const ReaderConfigurationInfo configurationInfo = readerInfo.getReaderConfigurationInfo();
			if (alreadyContained.contains(configurationInfo))
			{
				continue;
			}

			filtered += readerInfo;
			alreadyContained += configurationInfo;
		}
		return filtered;
	}

	return pInputList;
}
