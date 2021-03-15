/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateExtractCvcsFromEac1InputType.h"

#include "AppSettings.h"

using namespace governikus;

StateExtractCvcsFromEac1InputType::StateExtractCvcsFromEac1InputType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateExtractCvcsFromEac1InputType::run()
{
	Q_ASSERT(getContext()->getDidAuthenticateEac1());

	bool foundTerminalCvc = false, foundDvCvc = false;
	for (const auto& cvc : getContext()->getDidAuthenticateEac1()->getCvCertificates())
	{
		const auto& cvcAccessRole = cvc->getBody().getCHAT().getAccessRole();

		if (cvcAccessRole == AccessRole::AT)
		{
			if (foundTerminalCvc)
			{
				qCritical() << "More than one terminal certificate found in EAC1InputType";
				updateStatus(GlobalStatus::Code::Workflow_No_Unique_AtCvc);
				Q_EMIT fireAbort();
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
				Q_EMIT fireAbort();
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
		Q_EMIT fireAbort();
		return;
	}
	if (!foundDvCvc)
	{
		qCritical() << "No DV certificate found in EAC1InputType";
		updateStatus(GlobalStatus::Code::Workflow_No_Unique_DvCvc);
		Q_EMIT fireAbort();
		return;
	}

	getContext()->initCvcChainBuilder();

	Q_EMIT fireContinue();
}
