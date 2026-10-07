/**
 * Copyright (c) 2014-2026 Governikus Service GmbH, Germany
 */


#include "paos/retrieve/InitializeFramework.h"

#include "paos/PaosType.h"


using namespace governikus;


InitializeFramework::InitializeFramework()
	: PaosMessage(PaosType::INITIALIZE_FRAMEWORK)
{
}


InitializeFramework::~InitializeFramework() = default;
