/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetSelfAuthenticationData.h"

#include "SelfAuthenticationData.h"
#include "context/SelfAuthContext.h"


Q_DECLARE_LOGGING_CATEGORY(network)


using namespace governikus;


StateGetSelfAuthenticationData::StateGetSelfAuthenticationData(const QSharedPointer<WorkflowContext>& pContext)
	: StateGenericProviderCommunication(pContext)
{
}


QUrl StateGetSelfAuthenticationData::getRequestUrl() const
{
	return getContext()->getRefreshUrl();
}


void StateGetSelfAuthenticationData::setProgress() const
{
	const auto& context = getContext();
	Q_ASSERT(context);
	//: LABEL ALL_PLATFORMS
	context->setProgress(100, tr("Getting results from server"));
}


bool StateGetSelfAuthenticationData::isLoggingAllowed()
{
	return false;
}


void StateGetSelfAuthenticationData::handleNetworkReply(const QByteArray& pContent)
{
	if (const auto data = SelfAuthenticationData(pContent); data.isValid())
	{
		if (const auto& context = qobject_cast<SelfAuthContext*>(getContext()); context)
		{
			context->setSelfAuthenticationData(data);
		}

		Q_EMIT fireContinue();
		return;
	}

	qDebug() << "No valid data of self-authentication.";
	updateStatus(GlobalStatus::Code::Workflow_Server_Incomplete_Information_Provided);
	Q_EMIT fireAbort(FailureCode::Reason::Get_SelfAuthData_Invalid_Or_Empty);
}
