/*!
 * ReaderManagerPlugIn.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "ReaderManagerPlugIn.h"

using namespace governikus;

ReaderManagerPlugIn::ReaderManagerPlugIn(ReaderManagerPlugInType pPlugInType, bool pPlugInEnabled) : mInfo(pPlugInType, pPlugInEnabled)
{
}


ReaderManagerPlugIn::~ReaderManagerPlugIn()
{
}
