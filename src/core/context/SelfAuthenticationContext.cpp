/*!
 * SelfAuthenticationContext.cpp
 *
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "SelfAuthenticationContext.h"

using namespace governikus;

SelfAuthenticationContext::SelfAuthenticationContext(ActivationContext* pActivationContext)
	: AuthContext(pActivationContext)
	, mSelfAuthenticationData()
{
}


SelfAuthenticationContext::~SelfAuthenticationContext()
{
}
