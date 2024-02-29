/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "DestroyPaceChannelBuilder.h"


// TR-03119 D.3 Mapping on CCID
#define CCID_DESTROY_PACE_CHANNEL QByteArray::fromHex("FF9A0403")


using namespace governikus;


QByteArray DestroyPaceChannelBuilder::createASN1Struct() const
{
	// Command data according to PC/SC Part 10 amendment 1.1
	static const char INDEX_DESTROY_PACE_CHANNEL = 0x03;

	QByteArray commandData;
	commandData += INDEX_DESTROY_PACE_CHANNEL;
	commandData += '\0';
	commandData += '\0';
	return commandData;
}


CommandApdu DestroyPaceChannelBuilder::createASN1StructCcid() const
{
	return CommandApdu(CCID_DESTROY_PACE_CHANNEL);
}
