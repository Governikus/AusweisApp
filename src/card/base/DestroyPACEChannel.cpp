/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "DestroyPACEChannel.h"


using namespace governikus;


QByteArray DestroyPACEChannelBuilder::createCommandData()
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_DESTROY_PACE_CHANNEL = 0x03;
	QByteArray inputData;

	QByteArray commandData;
	commandData.append(INDEX_DESTROY_PACE_CHANNEL);
	commandData.append((inputData.size() >> 0) & 0xff);
	commandData.append((inputData.size() >> 8) & 0xff);
	commandData.append(inputData);
	return commandData;

}


CommandApdu DestroyPACEChannelBuilder::createCommandDataCcid()
{
	// boxing command according to TR-03119
	return CommandApdu(char(0xFF), char(0x9A), 0x04, 0x03);
}
