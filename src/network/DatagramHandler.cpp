/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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
