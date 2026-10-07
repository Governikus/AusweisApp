/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

#include "MockCardConnection.h"

using namespace governikus;


MockCardConnection::MockCardConnection(const ReaderInfo& pInfo)
	: CardConnection()
{
	mReaderInfo = pInfo;
}
