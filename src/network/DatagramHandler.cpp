/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "DatagramHandler.h"

#include "Initializer.h"

using namespace governikus;

static Initializer::Entry X([] {
			qRegisterMetaType<QHostAddress>("QHostAddress");
		});

DatagramHandler::DatagramHandler(bool)
{
}


DatagramHandler::~DatagramHandler()
{
}
