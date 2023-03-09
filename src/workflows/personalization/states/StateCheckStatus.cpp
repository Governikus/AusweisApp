/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckStatus.h"

#include "ReaderManager.h"

using namespace governikus;


StateCheckStatus::StateCheckStatus(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateCheckStatus::run()
{
	const auto smartInfo = Env::getSingleton<ReaderManager>()->getPlugInInfo(ReaderManagerPlugInType::SMART);
	if (smartInfo.isAvailable())
	{
		Q_EMIT fireContinue();
		return;
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Unavailable);
	qDebug() << "Smart-eID not available";
	Q_EMIT fireAbort(FailureCode::Reason::Check_Status_Unavailable);
}
