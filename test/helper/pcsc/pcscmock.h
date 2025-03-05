/**
 * Copyright (c) 2023-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#ifndef Q_OS_WIN
	#include <wintypes.h>
#endif

namespace governikus
{

void setResultGetCardStatus(LONG pReturnCode);
LONG getResultGetCardStatus();

} // namespace governikus
