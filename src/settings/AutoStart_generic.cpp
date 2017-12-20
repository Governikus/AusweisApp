/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#include "AutoStart.h"

#include <QDebug>


using namespace governikus;


bool AutoStart::enabled()
{
	return false;
}


void AutoStart::set(bool pEnabled)
{
	if (pEnabled)
	{
		qDebug() << "Autostart not supported on this system";
	}
}
