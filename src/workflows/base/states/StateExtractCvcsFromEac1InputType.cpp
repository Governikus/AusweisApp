/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
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
	const auto& context = getContext();
	const auto& eac1 = context->getDidAuthenticateEac1();
	Q_ASSERT(eac1);

	const auto& dvCvc = eac1->getCvCertificates({AccessRole::DV_no_f, AccessRole::DV_od});
	switch (dvCvc.size())
	{
		case 0:
			qCritical() << "No DV certificate found in EAC1InputType";
			updateStatus(GlobalStatus::Code::Workflow_No_Unique_DvCvc);
			Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_Dv_Missing);
			return;

		case 1:
			context->setDvCvc(dvCvc.constFirst());
			break;

		default:
			qCritical() << "More than one DV certificate found in EAC1InputType";
			updateStatus(GlobalStatus::Code::Workflow_No_Unique_DvCvc);
			Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_Dv);
			return;
	}

	const auto& terminalCvc = eac1->getCvCertificates({AccessRole::AT});
	switch (terminalCvc.size())
	{
		case 0:
			qCritical() << "No terminal certificate found in EAC1InputType";
			updateStatus(GlobalStatus::Code::Workflow_No_Unique_AtCvc);
			Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_At_Missing);
			return;

		case 1:
			context->initAccessRightManager(terminalCvc.constFirst());
			break;

		default:
			qCritical() << "More than one terminal certificate found in EAC1InputType";
			updateStatus(GlobalStatus::Code::Workflow_No_Unique_AtCvc);
			Q_EMIT fireAbort(FailureCode::Reason::Extract_Cvcs_From_Eac1_No_Unique_At);
			return;
	}

	context->initCvcChainBuilder();
	Q_EMIT fireContinue();
}
