/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QString>

namespace governikus
{

struct ContentItem
{
	QString mTitle;
	QString mContent;


	explicit ContentItem(const QString& pTitle = QString(), const QString& pContent = QString())
		: mTitle(pTitle)
		, mContent(pContent)
	{
	}


	bool operator==(const ContentItem& pOther) const
	{
		return mTitle == pOther.mTitle && mContent == pOther.mContent;
	}


};

} // namespace governikus
