/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief A configured filter used to retrieve readers
 */

#pragma once

#include "ReaderInfo.h"
#include "ReaderManagerPlugInInfo.h"

#include <QFlags>
#include <QList>

namespace governikus
{

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
		const ReaderFilter::FilterTypes mFilterType;
		const QList<ReaderManagerPlugInType> mPluginTypes;

	public:
		ReaderFilter();
		ReaderFilter(const ReaderFilter::FilterType pFilterType);
		explicit ReaderFilter(const QList<ReaderManagerPlugInType>& pPluginTypes);

		[[nodiscard]] QList<ReaderInfo> apply(const QList<ReaderInfo>& pInputList) const;
};

} // namespace governikus

Q_DECLARE_OPERATORS_FOR_FLAGS(governikus::ReaderFilter::FilterTypes)
