/*!
 * \brief A configured filter used to retrieve readers
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ReaderInfo.h"
#include "ReaderManagerPlugInInfo.h"

#include <QFlags>
#include <QVector>

namespace governikus
{

class ReaderManagerPlugIn;

class ReaderFilter
{
	public:
		enum FilterType
		{
			NoFilter = 0,
			PluginTypeFilter = 1,
			UniqueReaderTypes = 2
		};
		Q_DECLARE_FLAGS(FilterTypes, FilterType)

	private:
		ReaderFilter::FilterTypes mFilterType;
		const QVector<ReaderManagerPlugInType> mPluginTypes;

	public:
		ReaderFilter();
		ReaderFilter(const QVector<ReaderManagerPlugInType>& pPluginTypes);
		ReaderFilter(const ReaderFilter::FilterType pFilterType);

		QVector<ReaderManagerPlugIn*> apply(const QVector<ReaderManagerPlugIn*>& pPluginType) const;
		QVector<ReaderInfo> apply(const QVector<ReaderInfo>& pInputList) const;
};


}

Q_DECLARE_OPERATORS_FOR_FLAGS(governikus::ReaderFilter::FilterTypes)
