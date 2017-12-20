/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "GuiProfile.h"

using namespace governikus;

GuiProfile GuiProfile::mProfile;

GuiProfile::GuiProfile()
	: mShowWindow(false)
	, mDebugStyleSheet()
{
}


GuiProfile::~GuiProfile()
{
}


bool GuiProfile::getShowWindow() const
{
	return mShowWindow;
}


void GuiProfile::setShowWindow(bool pShow)
{
	mShowWindow = pShow;
}


const QString& GuiProfile::getDebugStyleSheet() const
{
	return mDebugStyleSheet;
}


void GuiProfile::setDebugStyleSheet(const QString& pStyleSheet)
{
	mDebugStyleSheet = pStyleSheet;
}


QLatin1String GuiProfile::getStyleSheetName()
{
#if defined(Q_OS_MACOS)
	return QLatin1String("macos.qss");

#else
	return QLatin1String("windows.qss");

#endif
}
