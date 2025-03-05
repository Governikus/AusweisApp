/**
 * Copyright (c) 2018-2025 Governikus GmbH & Co. KG, Germany
 */

#include "StateSendWhitelistSurvey.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "Survey.h"
#include "VolatileSettings.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateSendWhitelistSurvey::StateSendWhitelistSurvey(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractState(pContext)
	, GenericContextContainer(pContext)
{
}


void StateSendWhitelistSurvey::run()
{
#if !defined(QT_NO_DEBUG) || defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
	if (Env::getSingleton<VolatileSettings>()->isUsedAsSDK())
	{
		qCDebug(statemachine) << "Running as SDK. Ignoring whitelist survey.";
		Q_EMIT fireContinue();
		return;
	}

	auto* survey = Env::getSingleton<Survey>();
	if (!survey->isDeviceSurveyPending())
	{
		qCDebug(statemachine) << "No survey pending.";
		Q_EMIT fireContinue();
		return;
	}

	survey->setDeviceSurveyPending(false);
	survey->transmitSurvey();
#endif

	Q_EMIT fireContinue();
}
