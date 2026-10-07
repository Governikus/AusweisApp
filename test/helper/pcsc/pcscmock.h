/**
 * Copyright (c) 2023-2026 Governikus Service GmbH, Germany
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
