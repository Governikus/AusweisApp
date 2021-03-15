/*!
 * \copyright Copyright (c) 2016-2021 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandler.h"

#include "Initializer.h"

using namespace governikus;

INIT_FUNCTION([] {
			qRegisterMetaType<QHostAddress>("QHostAddress");
		})

DatagramHandler::DatagramHandler(bool)
{
}


DatagramHandler::~DatagramHandler()
{
}
