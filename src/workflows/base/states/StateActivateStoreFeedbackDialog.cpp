/**
 * Copyright (c) 2019-2026 Governikus Service GmbH, Germany
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
	if (getContext()->getStatus().isNoError() && !Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		Env::getSingleton<AppSettings>()->getGeneralSettings().setShowAppStoreRatingDialog(true);
	}
#endif
	Q_EMIT fireContinue();
}
