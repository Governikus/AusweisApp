/**
 * Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PinResetInformationModel.h"

#include "LanguageLoader.h"
#include "ProviderConfiguration.h"


using namespace governikus;


PinResetInformationModel::PinResetInformationModel()
	: QObject()
{
	const auto& providerConfiguration = Env::getSingleton<ProviderConfiguration>();
	connect(providerConfiguration, &ProviderConfiguration::fireUpdated, this, &PinResetInformationModel::fireUpdated);
}


QUrl PinResetInformationModel::getPinResetUrl() const
{
	const auto& config = Env::getSingleton<ProviderConfiguration>();
	const auto& info = config->getProviderInfo(QStringLiteral("pinResetService"));
	const auto& homepage = info.getAddress();

	if (homepage.isEmpty())
	{
		return tr("https://www.personalausweisportal.de/EN");
	}

	if (LanguageLoader::getLocaleCode() != QStringLiteral("de"))
	{
		return homepage + QStringLiteral("/en");
	}

	return homepage;
}


QString PinResetInformationModel::getNoPinAndNoPukHint() const
{
	//: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
	return tr("You cannot use the PUK to reset your previously set card PIN. If you forgot your card PIN, you can use the PIN Reset Service to request a new PIN.");
}


QString PinResetInformationModel::getRequestNewPinHint() const
{
	//: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
	return tr("Request a new card PIN free of charge to be able to use the eID function again.");
}


QString PinResetInformationModel::getActivateOnlineFunctionHint() const
{
	//: LABEL ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
	return tr("You can request activation of the eID function without charge.");
}


QString PinResetInformationModel::getActivateOnlineFunctionDescription() const
{
	return GlobalStatus(GlobalStatus::Code::Card_Pin_Deactivated).toErrorDescription();
}


QString PinResetInformationModel::getActivateOnlineFunctionActionText() const
{
	//: LABEL ALL_PLATFORMS
	return tr("Go to Activation Service");
}


QString PinResetInformationModel::getPinResetActionText() const
{
	//: LABEL ALL_PLATFORMS
	return tr("Go to PIN Reset Service");
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
