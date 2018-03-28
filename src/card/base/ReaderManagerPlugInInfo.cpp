/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "ReaderManagerPlugInInfo.h"

#include "Initializer.h"

using namespace governikus;

static Initializer::Entry X([] {
			qRegisterMetaType<ReaderManagerPlugInInfo>("ReaderManagerPlugInInfo");
			qRegisterMetaType<ReaderManagerPlugInType>("ReaderManagerPlugInType");
		});


ReaderManagerPlugInInfo::ReaderManagerPlugInInfo(ReaderManagerPlugInType pType, bool pEnabled, bool pAvailable)
	: mType(pType)
	, mValues()
	, mEnabled(pEnabled)
	, mAvailable(pAvailable)
	, mResponding(true)
{
}


#include "moc_ReaderManagerPlugInInfo.cpp"
