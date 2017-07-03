/*!
 * ReaderManagerPlugInInfo.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManagerPlugInInfo.h"

using namespace governikus;


ReaderManagerPlugInInfo::ReaderManagerPlugInInfo(ReaderManagerPlugInType pType, bool pEnabled, bool pAvailable)
	: mType(pType)
	, mValues()
	, mEnabled(pEnabled)
	, mAvailable(pAvailable)
{
}


#include "moc_ReaderManagerPlugInInfo.cpp"
