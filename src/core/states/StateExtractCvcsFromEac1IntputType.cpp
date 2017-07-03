/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#include "StateExtractCvcsFromEac1IntputType.h"

#include "AppSettings.h"

using namespace governikus;

StateExtractCvcsFromEac1IntputType::StateExtractCvcsFromEac1IntputType(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext)
{
}


void StateExtractCvcsFromEac1IntputType::run()
{
	Q_ASSERT(getContext()->getDidAuthenticateEac1());

	bool foundTerminalCvc = false, foundDvCvc = false;
	for (const auto& cvc : getContext()->getDidAuthenticateEac1()->getCvCertificates())
	{
		if (cvc->getBody().getCHAT().getAccessRole() == AccessRole::AT)
		{
			if (foundTerminalCvc)
			{
				qCritical() << "More than one terminal certificate found in EAC1InputType";
				setResult(Result::createCertChainInterruptedError(tr("No unique AT CVC")));
				Q_EMIT fireError();
				return;
			}
			foundTerminalCvc = true;
			getContext()->setTerminalCvc(cvc);
		}
		else if (cvc->getBody().getCHAT().getAccessRole() == AccessRole::DV_no_f)
		{
			if (foundDvCvc)
			{
				qCritical() << "More than one DV certificate found in EAC1InputType";
				setResult(Result::createCertChainInterruptedError(tr("No unique DV CVC")));
				Q_EMIT fireError();
				return;
			}
			foundDvCvc = true;
			getContext()->setDvCvc(cvc);
		}
	}

	if (!foundTerminalCvc)
	{
		qCritical() << "No terminal certificate found in EAC1InputType";
		setResult(Result::createCertChainInterruptedError(tr("No unique AT CVC")));
		Q_EMIT fireError();
		return;
	}
	if (!foundDvCvc)
	{
		qCritical() << "No DV certificate found in EAC1InputType";
		setResult(Result::createCertChainInterruptedError(tr("No unique DV CVC")));
		Q_EMIT fireError();
		return;
	}

	getContext()->initCvcChainBuilder();

	Q_EMIT fireSuccess();
}
