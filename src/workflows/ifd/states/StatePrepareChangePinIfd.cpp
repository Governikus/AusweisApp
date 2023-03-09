/**
 * Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StatePrepareChangePinIfd.h"

#include "ServerMessageHandler.h"
#include "pinpad/PinModify.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(ifd)


using namespace governikus;


StatePrepareChangePinIfd::StatePrepareChangePinIfd(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StatePrepareChangePinIfd::run()
{
	if (getContext()->getNewPin().isEmpty())
	{
		Q_EMIT fireEnterNewPacePin();
		return;
	}

	Q_EMIT fireContinue();
}
