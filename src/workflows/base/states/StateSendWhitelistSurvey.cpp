/*
 * \copyright Copyright (c) 2018-2023 Governikus GmbH & Co. KG, Germany
 */

#include "StateSendWhitelistSurvey.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "SurveyModel.h"
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

	const auto& surveyModel = Env::getSingleton<SurveyModel>();
	if (!surveyModel->isDeviceSurveyPending())
	{
		qCDebug(statemachine) << "No survey pending.";
		Q_EMIT fireContinue();
		return;
	}

	surveyModel->setDeviceSurveyPending(false);
	surveyModel->transmitSurvey();
#endif

	Q_EMIT fireContinue();
}
