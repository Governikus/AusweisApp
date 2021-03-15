/*
 * \copyright Copyright (c) 2019-2021 Governikus GmbH & Co. KG, Germany
 */

#include "StateActivateStoreFeedbackDialog.h"

#include "asn1/AccessRoleAndRight.h"
#include "AppSettings.h"
#include "VolatileSettings.h"

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
	if (getContext()->getStatus().isNoError() && settings.askForStoreFeedback() && !Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		settings.setRequestStoreFeedback(true);
	}
#endif
	Q_EMIT fireContinue();
}
