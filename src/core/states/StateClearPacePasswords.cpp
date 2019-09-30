/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateClearPacePasswords.h"


using namespace governikus;


StateClearPacePasswords::StateClearPacePasswords(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateClearPacePasswords::run()
{
	getContext()->resetPacePasswords();

	Q_EMIT fireContinue();
}
