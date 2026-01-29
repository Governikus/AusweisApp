/**
 * Copyright (c) 2015-2026 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthContext.h"

using namespace governikus;

SelfAuthContext::SelfAuthContext(bool pActivateUi)
	: AuthContext(Action::SELF_AUTH, pActivateUi)
	, mSelfAuthenticationData()
{
}
