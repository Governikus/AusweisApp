/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "paos/PaosMessage.h"

namespace governikus
{

class InitializeFramework
	: public PaosMessage
{
	public:
		explicit InitializeFramework();
		~InitializeFramework() override;
};

} // namespace governikus
