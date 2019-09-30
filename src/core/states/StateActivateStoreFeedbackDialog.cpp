/*
 * \copyright Copyright (c) 2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateActivateStoreFeedbackDialog.h"

#include "asn1/AccessRoleAndRight.h"
#include "AppSettings.h"

using namespace governikus;


StateActivateStoreFeedbackDialog::StateActivateStoreFeedbackDialog(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext, false)
	, GenericContextContainer(pContext)
{
}


void StateActivateStoreFeedbackDialog::run()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (getContext()->getStatus().isNoError() && settings.askForStoreFeedback())
	{
		settings.setRequestStoreFeedback(true);
	}
#endif
	Q_EMIT fireContinue();
}
