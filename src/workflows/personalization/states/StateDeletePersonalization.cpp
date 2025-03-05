/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateDeletePersonalization.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>
#include <QSignalBlocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateDeletePersonalization::StateDeletePersonalization(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StateDeletePersonalization::run()
{
	const auto func = []() -> QVariant {
				const auto& smartManager = SmartManager::get();
				if (!smartManager->deletePersonalization())
				{
					qCDebug(card_smart) << "Deletion of Smart-eID personalization failed";
					return false;
				}

				qCDebug(card_smart) << "Successfully deleted the Smart-eID personalization";
				return true;
			};

	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateDeletePersonalization::onCommandDone);
}


void StateDeletePersonalization::onCommandDone(const QVariant& pResult)
{
	Q_ASSERT(ReaderManager::isResultType<bool>(pResult));

	if (getContext()->isWorkflowCancelledInState())
	{
		Q_EMIT fireAbort(FailureCode::Reason::Delete_Personalization_User_Cancelled);
		return;
	}

	if (pResult.value<bool>())
	{
		Q_EMIT fireContinue();
		return;
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
	Q_EMIT fireAbort(FailureCode::Reason::Delete_Personalization_Failed);

}


void StateDeletePersonalization::onUserCancelled()
{
	const QSignalBlocker blocker(this);
	AbstractState::onUserCancelled();
}
