/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "DatagramHandler.h"


using namespace governikus;


void DatagramHandler::registerMetaTypes()
{
	static bool registered = false;
	if (!registered)
	{
		qRegisterMetaType<QHostAddress>("QHostAddress");
		registered = true;
	}
}


DatagramHandler::~DatagramHandler()
{
}
