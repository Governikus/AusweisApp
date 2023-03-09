/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PersonalizationModel.h"

#include "LanguageLoader.h"
#if __has_include("controller/PersonalizationController.h")
	#include "AppSettings.h"
	#include "SecureStorage.h"
	#include "controller/PersonalizationController.h"
#endif

#include <QDate>

using namespace governikus;


PersonalizationModel::PersonalizationModel()
	: AuthModel()
{
}


void PersonalizationModel::startWorkflow()
{
#if __has_include("controller/PersonalizationController.h")
	const bool useTestUri = Env::getSingleton<AppSettings>()->getGeneralSettings().useSelfAuthTestUri();
	const auto& url = Env::getSingleton<SecureStorage>()->getSmartPersonalizationUrl(useTestUri);
	Q_EMIT fireStartWorkflow(PersonalizationController::createWorkflowRequest(url));
#endif
}


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
	QDate restrictionDate = QDate::currentDate();
#if __has_include("context/PersonalizationContext.h")
	if (mContext)
	{
		restrictionDate = restrictionDate.addDays(qAbs(mContext->getRemainingDays()));
	}
#endif
	const auto& usedLocale = LanguageLoader::getInstance().getUsedLocale();
	return usedLocale.toString(restrictionDate, QStringLiteral("d. MMMM yyyy"));
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
	}
}


#endif


QVector<ReaderManagerPlugInType> PersonalizationModel::getSupportedReaderPlugInTypes() const
{
	auto plugins = WorkflowModel::getSupportedReaderPlugInTypes();
	plugins.removeOne(ReaderManagerPlugInType::SMART);
	return plugins;
}


void PersonalizationModel::onTranslationChanged()
{
	Q_EMIT fireRestrictionDateChanged();
}
