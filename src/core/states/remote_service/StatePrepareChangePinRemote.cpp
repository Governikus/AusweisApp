/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#include "StatePrepareChangePinRemote.h"

#include "PinModify.h"
#include "ServerMessageHandler.h"

#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(remote_device)


using namespace governikus;


StatePrepareChangePinRemote::StatePrepareChangePinRemote(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StatePrepareChangePinRemote::run()
{
	if (getContext()->getNewPin().isEmpty())
	{
		Q_EMIT fireEnterNewPacePin();
		return;
	}

	Q_EMIT fireContinue();
}
