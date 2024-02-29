/**
 * Copyright (c) 2015-2024 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthContext.h"

using namespace governikus;

SelfAuthContext::SelfAuthContext(bool pActivateUi)
	: AuthContext(Action::SELF, pActivateUi)
	, mSelfAuthenticationData()
{
}
