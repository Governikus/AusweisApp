/*!
 * GuiProfile.cpp
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "GuiProfile.h"

using namespace governikus;

GuiProfile GuiProfile::mProfile;

GuiProfile::GuiProfile()
	: mScreenSize(ScreenSize::SMALL)
	, mShowWindow(false)
	, mShowMainTabs(true)
	, mWindowSize()
	, mDebugStyleSheet()
{
}


GuiProfile::~GuiProfile()
{
}


ScreenSize GuiProfile::getScreenSize() const
{
	return mScreenSize;
}


void GuiProfile::setScreenSize(ScreenSize pScreenSize)
{
	mScreenSize = pScreenSize;
}


bool GuiProfile::isSmallScreen() const
{
	return mScreenSize == ScreenSize::SMALL ? true : false;
}


bool GuiProfile::isMediumScreen() const
{
	return mScreenSize == ScreenSize::MEDIUM ? true : false;
}


bool GuiProfile::isLargeScreen() const
{
	return mScreenSize == ScreenSize::LARGE ? true : false;
}


bool GuiProfile::getShowWindow() const
{
	return mShowWindow;
}


void GuiProfile::setShowWindow(bool pShow)
{
	mShowWindow = pShow;
}


bool GuiProfile::getShowMainTabs() const
{
	return mShowMainTabs;
}


void GuiProfile::setShowMainTabs(bool pShow)
{
	mShowMainTabs = pShow;
}


QSize GuiProfile::getWindowSize() const
{
	return mWindowSize;
}


void GuiProfile::setWindowSize(QSize pWindowSize)
{
	mWindowSize = pWindowSize;
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
#if defined(Q_OS_OSX)
	return QLatin1String("macos.qss");

#else
	return QLatin1String("windows.qss");

#endif
}
