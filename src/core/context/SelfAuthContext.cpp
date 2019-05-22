/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
 */

#include "SelfAuthContext.h"

using namespace governikus;

SelfAuthContext::SelfAuthContext()
	: AuthContext(QSharedPointer<ActivationContext>())
	, mSelfAuthenticationData()
{
}
