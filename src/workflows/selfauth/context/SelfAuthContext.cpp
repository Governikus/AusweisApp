/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthContext.h"

using namespace governikus;

SelfAuthContext::SelfAuthContext(bool pActivateUi)
	: AuthContext(Action::SELF, pActivateUi)
	, mSelfAuthenticationData()
{
}
