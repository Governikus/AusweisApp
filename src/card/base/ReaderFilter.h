/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "ReaderInfo.h"
#include "ReaderManagerPluginInfo.h"

#include <QFlags>
#include <QList>


namespace governikus
{

class ReaderFilter
{
	private:
		const bool mfilter;
		const QList<ReaderManagerPluginType> mPluginTypes;

	public:
		ReaderFilter();
		explicit ReaderFilter(const QList<ReaderManagerPluginType>& pPluginTypes);

		[[nodiscard]] QList<ReaderInfo> apply(const QList<ReaderInfo>& pInputList) const;
};

} // namespace governikus
