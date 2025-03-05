/**
 * Copyright (c) 2020-2025 Governikus GmbH & Co. KG, Germany
 */

#include "MockCardConnection.h"

using namespace governikus;


MockCardConnection::MockCardConnection(const ReaderInfo& pInfo)
	: CardConnection()
{
	mReaderInfo = pInfo;
}
