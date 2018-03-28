/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DestroyPACEChannel.h"


using namespace governikus;


QByteArray DestroyPACEChannelBuilder::createCommandData()
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_DESTROY_PACE_CHANNEL = 0x03;

	QByteArray commandData;
	commandData += INDEX_DESTROY_PACE_CHANNEL;
	commandData += '\0';
	commandData += '\0';
	return commandData;
}


CommandApdu DestroyPACEChannelBuilder::createCommandDataCcid()
{
	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x03);
}
