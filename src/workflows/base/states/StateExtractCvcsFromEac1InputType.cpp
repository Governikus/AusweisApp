/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#include "StateExtractCvcsFromEac1InputType.h"

#include "AppSettings.h"

using namespace governikus;

StateExtractCvcsFromEac1InputType::StateExtractCvcsFromEac1InputType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateExtractCvcsFromEac1InputType::run()
{
	Q_ASSERT(getContext()->getDidAuthenticateEac1());

	bool foundTerminalCvc = false;
	bool foundDvCvc = false;
	for (const auto& cvc : getContext()->getDidAuthenticateEac1()->getCvCertificates())
	{
		const auto& cvcAccessRole = cvc->getBody().getCHAT().getAccessRole();

		if (cvcAccessRole == AccessRole::AT)
		{
			if (foundTerminalCvc)
			{
				qCritical() << "More than one terminal certificate found in EAC1InputType";
				updateStatus(GlobalStatus::Code::Workflow_No_Unique_AtCvc);
				Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_At);
				return;
			}
			foundTerminalCvc = true;
			getContext()->initAccessRightManager(cvc);
		}
		else if (cvcAccessRole == AccessRole::DV_no_f || cvcAccessRole == AccessRole::DV_od)
		{
			if (foundDvCvc)
			{
				qCritical() << "More than one DV certificate found in EAC1InputType";
				updateStatus(GlobalStatus::Code::Workflow_No_Unique_DvCvc);
				Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_Dv);
				return;
			}
			foundDvCvc = true;
			getContext()->setDvCvc(cvc);
		}
	}

	if (!foundTerminalCvc)
	{
		qCritical() << "No terminal certificate found in EAC1InputType";
		updateStatus(GlobalStatus::Code::Workflow_No_Unique_AtCvc);
		Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_At_Missing);
		return;
	}
	if (!foundDvCvc)
	{
		qCritical() << "No DV certificate found in EAC1InputType";
		updateStatus(GlobalStatus::Code::Workflow_No_Unique_DvCvc);
		Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_Dv_Missing);
		return;
	}

	getContext()->initCvcChainBuilder();

	Q_EMIT fireContinue();
}
