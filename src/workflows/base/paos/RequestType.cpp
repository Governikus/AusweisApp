/*!
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "RequestType.h"

using namespace governikus;

RequestType::RequestType(PaosType pType)
	: PaosMessage(pType)
{
}


RequestType::~RequestType() = default;
