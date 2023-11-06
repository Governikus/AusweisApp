/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateDeleteApplet.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>
#include <QSignalBlocker>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateDeleteApplet::StateDeleteApplet(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer<PersonalizationContext>(pContext)
{
}


void StateDeleteApplet::run()
{
	const auto func = [this]() {
				const auto& context = getContext();
				const auto& smartManager = SmartManager::get();
				const int initialProgress = context->getProgressValue();
				const auto& progressHandler = [this, &context, &initialProgress](int pProgress) {
							QMetaObject::invokeMethod(this, [pProgress, &context, &initialProgress]{
									//: LABEL ANDROID IOS
									context->setProgress(pProgress, tr("Cleaning up old Smart-eID"), initialProgress, 50);
								}, Qt::QueuedConnection);
						};
				//: LABEL ANDROID IOS
				context->setProgress(initialProgress, tr("Cleaning up old Smart-eID"));
				return QVariant::fromValue(smartManager->deleteSmart(progressHandler));
			};

	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand(func, this, &StateDeleteApplet::onCommandDone);
}


void StateDeleteApplet::handleEidServiceResult(const EidServiceResult& pResult)
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
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Fail);
			return;

		case EidServiceResult::UNSUPPORTED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Unsupported);
			return;

		case EidServiceResult::OVERLOAD_PROTECTION:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Overload);
			return;

		case EidServiceResult::UNDER_MAINTENANCE:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Maintenance);
			return;

		case EidServiceResult::NFC_NOT_ACTIVATED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Nfc_Disabled);
			return;

		case EidServiceResult::INTEGRITY_CHECK_FAILED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Integrity_Check_Failed);
			return;

		case EidServiceResult::NOT_AUTHENTICATED:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Not_Authenticated);
			return;

		case EidServiceResult::NETWORK_CONNECTION_ERROR:
			updateStatus(GlobalStatus::Code::Workflow_Smart_eID_Applet_Preparation_Failed);
			Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_Service_Response_Network_Connection_Error);
			return;
	}
}


void StateDeleteApplet::onCommandDone(const QVariant& pResult)
{
	Q_ASSERT(ReaderManager::isResultType<EidServiceResult>(pResult));

	if (getContext()->isWorkflowCancelledInState())
	{
		Q_EMIT fireAbort(FailureCode::Reason::Delete_Smart_User_Cancelled);
		return;
	}

	const auto& result = pResult.value<EidServiceResult>();
	if (result == EidServiceResult::SUCCESS)
	{
		qCDebug(card_smart) << "Successfully deleted Smart-eID";
	}
	else
	{
		qCDebug(card_smart) << "Deletion of Smart-eID failed";
	}
	handleEidServiceResult(result);
}


void StateDeleteApplet::onUserCancelled()
{
	const QSignalBlocker blocker(this);
	AbstractState::onUserCancelled();
}
