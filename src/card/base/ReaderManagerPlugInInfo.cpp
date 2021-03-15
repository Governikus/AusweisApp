/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugInInfo.h"

#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<ReaderManagerPlugInInfo>("ReaderManagerPlugInInfo");
			qRegisterMetaType<ReaderManagerPlugInType>("ReaderManagerPlugInType");
		})


ReaderManagerPlugInInfo::ReaderManagerPlugInInfo(ReaderManagerPlugInType pType, bool pEnabled, bool pAvailable)
	: mType(pType)
	, mValues()
	, mEnabled(pEnabled)
	, mAvailable(pAvailable)
{
}


#include "moc_ReaderManagerPlugInInfo.cpp"
