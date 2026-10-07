/**
 * Copyright (c) 2020-2026 Governikus Service GmbH, Germany
 */

#pragma once


#include "CardConnection.h"
#include "ReaderInfo.h"


namespace governikus
{

class MockCardConnection
	: public CardConnection
{
	Q_OBJECT

	public:
		MockCardConnection() = default;
		explicit MockCardConnection(const ReaderInfo& pInfo);
};

} // namespace governikus
