/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "StateClearPacePasswords.h"


using namespace governikus;


StateClearPacePasswords::StateClearPacePasswords(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateClearPacePasswords::run()
{
	getContext()->resetPacePasswords();

	Q_EMIT fireContinue();
}
