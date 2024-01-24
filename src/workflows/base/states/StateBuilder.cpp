/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateBuilder.h"


using namespace governikus;


QString StateBuilder::getUnqualifiedClassName(const char* const pName)
{
	QString className = QString::fromLatin1(pName);
	if (className.contains(QLatin1Char(':')))
	{
		className = className.mid(className.lastIndexOf(QLatin1Char(':')) + 1);
	}
	return className;
}
