/**
 * Copyright (c) 2020-2024 Governikus GmbH & Co. KG, Germany
 */

#include "MockCardConnection.h"

using namespace governikus;


MockCardConnection::MockCardConnection(const ReaderInfo& pInfo)
	: CardConnection()
{
	mReaderInfo = pInfo;
}
