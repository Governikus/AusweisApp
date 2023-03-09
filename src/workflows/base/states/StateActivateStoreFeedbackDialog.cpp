/**
 * Copyright (c) 2019-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateActivateStoreFeedbackDialog.h"

#include "AppSettings.h"
#include "VolatileSettings.h"
#include "asn1/AccessRoleAndRight.h"

using namespace governikus;


StateActivateStoreFeedbackDialog::StateActivateStoreFeedbackDialog(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateActivateStoreFeedbackDialog::run()
{
#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	auto& settings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	if (getContext()->getStatus().isNoError() && settings.askForStoreFeedback() && !Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		settings.setRequestStoreFeedback(true);
	}
#endif
	Q_EMIT fireContinue();
}
