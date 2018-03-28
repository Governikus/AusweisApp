/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#include "GuiProfile.h"

using namespace governikus;

GuiProfile GuiProfile::mProfile;


GuiProfile::GuiProfile()
	: mShowWindow(false)
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
