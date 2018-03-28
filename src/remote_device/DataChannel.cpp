/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "DataChannel.h"


using namespace governikus;


DataChannel::DataChannel()
{
}


DataChannel::~DataChannel()
{
}


const QString& DataChannel::getId() const
{
	static const QString EMPTY_STRING;

	return EMPTY_STRING;
}
