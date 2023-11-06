/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateCheckApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateCheckApplet::StateCheckApplet(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StateCheckApplet::run()
{
	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
			return QVariant::fromValue(SmartManager::get()->status());
		}, this, &StateCheckApplet::onCommandDone);
}


void StateCheckApplet::onCommandDone(const QVariant& pResult)
{
	Q_ASSERT(ReaderManager::isResultType<EidStatus>(pResult));

	switch (pResult.value<EidStatus>())
	{
		case EidStatus::INTERNAL_ERROR:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Check_Applet_Internal_Error);
			return;

		case EidStatus::NO_PROVISIONING:
			Q_EMIT fireInstallApplet();
			return;

		case EidStatus::CERT_EXPIRED:
		case EidStatus::PERSONALIZED:
			Q_EMIT fireDeletePersonalization();
			return;

		case EidStatus::NO_PERSONALIZATION:
			Q_EMIT fireContinue();
			return;

		case EidStatus::UNUSABLE:
			Q_EMIT fireDeleteApplet();
			return;

	}
}
