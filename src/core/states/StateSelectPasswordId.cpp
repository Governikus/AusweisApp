/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateSelectPasswordId.h"


using namespace governikus;


StateSelectPasswordId::StateSelectPasswordId(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateSelectPasswordId::run()
{
	const bool canAllowed = getContext()->isCanAllowedMode();
	qDebug() << "CAN allowed:" << canAllowed;
	if (canAllowed)
	{
		Q_EMIT firePasswordIdCAN();
		return;
	}

	Q_EMIT fireContinue();
}
