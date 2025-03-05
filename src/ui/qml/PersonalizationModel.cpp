/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#include "PersonalizationModel.h"

#include "GlobalStatus.h"
#if __has_include("controller/PersonalizationController.h")
	#include "AppSettings.h"
	#include "SecureStorage.h"
	#include "controller/PersonalizationController.h"
#endif

using namespace governikus;


PersonalizationModel::PersonalizationModel()
	: AuthModel()
{
}


#if __has_include("controller/PersonalizationController.h")
void PersonalizationModel::startWorkflow()
{

	const bool useTestUri = Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();
	const auto& url = Env::getSingleton<SecureStorage>()->getSmartPersonalizationUrl(useTestUri);
	Q_EMIT fireStartWorkflow(PersonalizationController::createWorkflowRequest(url));
}


#endif


QString PersonalizationModel::getBlockingCode() const
{
#if __has_include("context/PersonalizationContext.h")
	if (mContext)
	{
		return mContext->getBlockingCode();
	}
#endif
	return QString();
}


int PersonalizationModel::getRemainingAttempts() const
{
#if __has_include("context/PersonalizationContext.h")
	if (mContext)
	{
		return mContext->getRemainingAttempts();
	}
#endif
	return -1;
}


QString PersonalizationModel::getRestrictionDate() const
{
#if __has_include("context/PersonalizationContext.h")
	if (mContext)
	{
		return mContext->getRestrictionDate();
	}
#endif
	return QStringLiteral("N/A");
}


QString PersonalizationModel::getBlockingPeriodMessage() const
{
	const auto& restrictionDate = getRestrictionDate();
	const GlobalStatus status = {GlobalStatus::Code::Workflow_Smart_eID_Personalization_Denied,
								 {GlobalStatus::ExternalInformation::PERSONALIZATION_RESTRICTION_DATE, restrictionDate}
	};
	return status.toErrorDescription();
}


bool PersonalizationModel::isApplet() const
{
#if __has_include("context/PersonalizationContext.h")
	return mContext->getSmartEidType() == SmartEidType::APPLET;

#else
	return false;

#endif
}


#if __has_include("context/PersonalizationContext.h")
void PersonalizationModel::resetPersonalizationContext(const QSharedPointer<PersonalizationContext>& pContext)
{
	mContext = pContext;
	AuthModel::resetAuthContext(pContext);

	if (pContext)
	{
		connect(mContext.data(), &PersonalizationContext::fireBlockingCodeChanged, this, &PersonalizationModel::fireBlockingCodeChanged);
		connect(mContext.data(), &PersonalizationContext::fireRemainingAttemptsChanged, this, &PersonalizationModel::fireRemainingAttemptsChanged);
		connect(mContext.data(), &PersonalizationContext::fireRemainingDaysChanged, this, &PersonalizationModel::fireRestrictionDateChanged);
		connect(mContext.data(), &PersonalizationContext::fireServiceInformationChanged, this, &PersonalizationModel::fireIsAppletChanged);
	}
}


#endif


QList<ReaderManagerPluginType> PersonalizationModel::getSupportedReaderPluginTypes() const
{
	auto plugins = WorkflowModel::getSupportedReaderPluginTypes();
	plugins.removeOne(ReaderManagerPluginType::SMART);
	return plugins;
}


void PersonalizationModel::onTranslationChanged()
{
	Q_EMIT fireRestrictionDateChanged();
}
