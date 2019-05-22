/*
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#include "StateSendWhitelistSurvey.h"

#include "AppSettings.h"
#include "ReaderManager.h"
#include "SurveyHandler.h"

Q_DECLARE_LOGGING_CATEGORY(statemachine)

using namespace governikus;


StateSendWhitelistSurvey::StateSendWhitelistSurvey(const QSharedPointer<WorkflowContext>& pContext)
	: AbstractGenericState(pContext, false)
{
}


void StateSendWhitelistSurvey::run()
{
#if !defined(QT_NO_DEBUG) || defined(Q_OS_ANDROID)
	if (Env::getSingleton<AppSettings>()->isUsedAsSDK())
	{
		qCDebug(statemachine) << "Running as SDK. Ignoring whitelist survey.";
		Q_EMIT fireContinue();
		return;
	}

	if (!Env::getSingleton<AppSettings>()->getGeneralSettings().isDeviceSurveyPending())
	{
		qCDebug(statemachine) << "No survey pending.";
		Q_EMIT fireContinue();
		return;
	}

	Env::getSingleton<AppSettings>()->getGeneralSettings().setDeviceSurveyPending(false);

	const auto authContext = getContext();
	if (authContext->getDidAuthenticateEac1() == nullptr || authContext->getEffectiveAccessRights().isEmpty() ||
			authContext->getStatus().isError())
	{
		qWarning() << "Authentication was not completed successfully, cannot send survey to whitelist server.";
		Q_ASSERT(false);
		Q_EMIT fireAbort();
		return;
	}

	const QString& readerName = authContext->getReaderName();
	if (readerName.isEmpty())
	{
		qWarning() << "No reader information available, cannot send survey to whitelist server.";
		Q_ASSERT(false);
		Q_EMIT fireAbort();
		return;
	}

	const ReaderInfo& readerInfo = Env::getSingleton<ReaderManager>()->getReaderInfo(readerName);
	SurveyHandler().sendSurvey(readerInfo.getMaxApduLength());
#endif

	Q_EMIT fireContinue();
}
