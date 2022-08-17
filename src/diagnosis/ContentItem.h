/*
 * \copyright Copyright (c) 2018-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

struct ContentItem
{
	ContentItem(const QString& pTitle, const QString& pContent)
		: mTitle(pTitle)
		, mContent(pContent)
	{
	}


	QString mTitle;
	QString mContent;
};

} // namespace governikus
