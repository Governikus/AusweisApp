/*!
 * ReaderManagerPlugInInfo.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManagerPlugInInfo.h"

using namespace governikus;


ReaderManagerPlugInInfo::ReaderManagerPlugInInfo(ReaderManagerPlugInType pType, bool pEnabled)
	: mType(pType)
	, mValues()
	, mEnabled(pEnabled)
{
}


#include "moc_ReaderManagerPlugInInfo.cpp"
