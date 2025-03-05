/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateUpdateSupportInfo.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>
#include <QSignalBlocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateUpdateSupportInfo::StateUpdateSupportInfo(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StateUpdateSupportInfo::run()
{
	const auto func = [this]() {
				const auto& context = getContext();
				const auto& smartManager = SmartManager::get();
				//: LABEL ANDROID IOS
				context->setProgress(context->getProgressValue(), tr("Checking Smart-eID status"));

				const auto& supportResult = smartManager->updateSupportInfo();
				if (supportResult.mResult != EidServiceResult::SUCCESS)
				{
					qCDebug(card_smart) << "updateSupportInfo() failed with" << supportResult.mResult;
					return QVariant::fromValue(supportResult.mResult);
				}

				return QVariant::fromValue(supportResult.mStatus);
			};

	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateUpdateSupportInfo::onCommandDone);
}


void StateUpdateSupportInfo::handleEidServiceResult(const EidServiceResult& pResult)
{
	switch (pResult)
	{
		case EidServiceResult::SUCCESS:
			Q_EMIT fireContinue();
			return;

		case EidServiceResult::UNDEFINED:
		case EidServiceResult::INFO:
		case EidServiceResult::WARN:
		case EidServiceResult::ERROR:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Fail);
			return;

		case EidServiceResult::UNSUPPORTED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Unsupported);
			return;

		case EidServiceResult::OVERLOAD_PROTECTION:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Overload);
			return;

		case EidServiceResult::UNDER_MAINTENANCE:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Maintenance);
			return;

		case EidServiceResult::NFC_NOT_ACTIVATED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Nfc_Disabled);
			return;

		case EidServiceResult::INTEGRITY_CHECK_FAILED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Integrity_Check_Failed);
			return;

		case EidServiceResult::NOT_AUTHENTICATED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Not_Authenticated);
			return;

		case EidServiceResult::NETWORK_CONNECTION_ERROR:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Service_Response_Network_Connection_Error);
			return;
	}
}


void StateUpdateSupportInfo::handleEidSupportStatus(const EidSupportStatus& pStatus)
{
	if (pStatus == EidSupportStatus::UP_TO_DATE)
	{
		Q_EMIT fireContinue();
		return;
	}

	if (pStatus == EidSupportStatus::UPDATE_AVAILABLE)
	{
		qCDebug(card_smart) << "Update available, delete the Smart-eID first";
		Q_EMIT fireUpdateAvailable();
		return;
	}

	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
	Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_Call_Failed);
}


void StateUpdateSupportInfo::onCommandDone(const QVariant& pResult)
{
	if (getContext()->isWorkflowCancelledInState())
	{
		Q_EMIT fireAbort(FailureCode::Reason::Update_Support_Info_User_Cancelled);
		return;
	}

	if (ReaderManager::isResultType<EidServiceResult>(pResult))
	{
		handleEidServiceResult(pResult.value<EidServiceResult>());
	}
	else if (ReaderManager::isResultType<EidSupportStatus>(pResult))
	{
		handleEidSupportStatus(pResult.value<EidSupportStatus>());
	}
	else
	{
		Q_ASSERT(false);
	}
}


void StateUpdateSupportInfo::onUserCancelled()
{
	const QSignalBlocker blocker(this);
	AbstractState::onUserCancelled();
}
