/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateGetServiceInformation.h"

#include "ReaderManager.h"
#include "SmartManager.h"

#include <QLoggingCategory>


using namespace governikus;


Q_DECLARE_LOGGING_CATEGORY(card_smart)


StateGetServiceInformation::StateGetServiceInformation(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateGetServiceInformation::run()
{
	*this << Env::getSingleton<ReaderManager>()->callExecuteCommand([] {
			return QVariant::fromValue(SmartManager::get()->serviceInformation());
		}, this, &StateGetServiceInformation::onCommandDone);
}


void StateGetServiceInformation::onCommandDone(const QVariant& pResult)
{
	const auto [result, smartEidType, challengeType, libVersion] = pResult.value<ServiceInformationResult>();

	if (result == EidServiceResult::SUCCESS)
	{
		getContext()->setServiceInformation(smartEidType, QString::fromStdString(challengeType), QString::fromStdString(libVersion));
		Q_EMIT fireContinue();
		return;
	}

	qCWarning(card_smart) << "ServiceInformation query failed";
	updateStatus(GlobalStatus::Code::Workflow_Smart_eID_ServiceInformation_Query_Failed);
	Q_EMIT fireAbort(FailureCode::Reason::Smart_ServiceInformation_Query_Failed);
}
