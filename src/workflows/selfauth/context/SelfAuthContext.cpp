/*!
 * \copyright Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthContext.h"

using namespace governikus;

SelfAuthContext::SelfAuthContext()
	: AuthContext(Action::SELF, QSharedPointer<ActivationContext>())
	, mSelfAuthenticationData()
{
}
