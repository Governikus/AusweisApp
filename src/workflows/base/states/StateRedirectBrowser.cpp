/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateRedirectBrowser.h"

#include <QDebug>

using namespace governikus;


StateRedirectBrowser::StateRedirectBrowser(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateRedirectBrowser::run()
{
	if (const auto& handler = getContext()->getBrowserHandler(); handler)
	{
		if (const auto& error = handler(getContext()); !error.isEmpty())
		{
			qCritical() << "Cannot send page to caller:" << error;
			updateStatus({GlobalStatus::Code::Workflow_Browser_Transmission_Error, {GlobalStatus::ExternalInformation::ACTIVATION_ERROR, error}
					});
			Q_EMIT fireAbort(FailureCode::Reason::Browser_Send_Failed);
			return;
		}
	}

	Q_EMIT fireContinue();
}
