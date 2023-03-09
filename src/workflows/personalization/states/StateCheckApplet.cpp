/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"


using namespace governikus;


StateCheckApplet::StateCheckApplet(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StateCheckApplet::run()
{
	mConnections += Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
			return QVariant::fromValue(SmartManager::get()->status());
		}, this, &StateCheckApplet::onCommandDone);
}


void StateCheckApplet::onCommandDone(const QVariant& pResult)
{
	switch (pResult.value<EidStatus>())
	{
		case EidStatus::INTERNAL_ERROR:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Check_Applet_Error);
			return;

		case EidStatus::UNAVAILABLE:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Unavailable);
			Q_EMIT fireAbort(FailureCode::Reason::Check_Applet_Unavailable);
			return;

		case EidStatus::NO_PERSONALIZATION:
			Q_EMIT fireContinue();
			return;

		case EidStatus::NO_PROVISIONING:
		case EidStatus::APPLET_UNUSABLE:
		case EidStatus::PERSONALIZED:
			Q_EMIT fireFurtherStepRequired();
			return;
	}
}
