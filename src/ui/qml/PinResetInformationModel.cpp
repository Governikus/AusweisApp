/**
 * Copyright (c) 2022-2026 Governikus GmbH & Co. KG, Germany
 */

#include "PinResetInformationModel.h"

#include "LanguageLoader.h"
#include "ProviderConfiguration.h"


using namespace governikus;


PinResetInformationModel::PinResetInformationModel()
	: QObject()
{
	const auto* providerConfiguration = Env::getSingleton<ProviderConfiguration>();
	connect(providerConfiguration, &ProviderConfiguration::fireUpdated, this, &PinResetInformationModel::fireUpdated);
}


bool PinResetInformationModel::hasPinResetService() const
{
	const auto* config = Env::getSingleton<ProviderConfiguration>();
	const auto& info = config->getProviderInfo(QStringLiteral("pinResetService"));
	return !info.getAddress().isEmpty();
}


QUrl PinResetInformationModel::getAdministrativeSearchUrl() const
{
	if (LanguageLoader::getLocaleCode() == QLatin1String("de"))
	{
		return QUrl(QStringLiteral("https://servicesuche.bund.de"));
	}
	return QUrl(QStringLiteral("https://servicesuche.bund.de/#/en"));
}


QUrl PinResetInformationModel::getPinResetActivationUrl() const
{
	return QUrl(QStringLiteral("https://www.pin-ruecksetzbrief-bestellen.de/aktivierung"));
}


QUrl PinResetInformationModel::getPinResetUrl() const
{
	const auto* config = Env::getSingleton<ProviderConfiguration>();
	const auto& info = config->getProviderInfo(QStringLiteral("pinResetService"));
	const auto& homepage = info.getAddress();

	if (homepage.isEmpty())
	{
		return getAdministrativeSearchUrl();
	}

	if (LanguageLoader::getLocaleCode() != QLatin1String("de"))
	{
		return QUrl(homepage + QStringLiteral("/en"));
	}

	return QUrl(homepage);
}


QString PinResetInformationModel::getActivateOnlineFunctionForPRSHint() const
{
	//: ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
	return tr("You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website to activate the eID function.");
}


QString PinResetInformationModel::getActivateOnlineFunctionAtAuthorityHint() const
{
	//: ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
	return tr("You may turn to your competent authority to activate the eID function.");
}


QString PinResetInformationModel::getActivateOnlineFunctionActionText() const
{
	return hasPinResetService() ? getResetPinWithPRSActionText() : getResetPinAtAuthorityActionText();
}


QString PinResetInformationModel::getActivateOnlineFunctionDescription() const
{
	return GlobalStatus(GlobalStatus::Code::Card_Pin_Deactivated).toErrorDescription();
}


QString PinResetInformationModel::getResetPinWithPRSActionText() const
{
	//: ALL_PLATFORMS
	return tr("Request PIN Reset Letter");
}


QString PinResetInformationModel::getResetPinAtAuthorityActionText() const
{
	//: ALL_PLATFORMS
	return tr("Find competent authority");
}


QString PinResetInformationModel::getResetPinWithPRSHintTitle() const
{
	//: ALL_PLATFORMS
	return tr("Online via PIN Reset Service");
}


QString PinResetInformationModel::getResetPinAtAuthorityHintTitle() const
{
	//: ALL_PLATFORMS
	return tr("At your competent authority");
}


QString PinResetInformationModel::getResetPinWithPRSHint() const
{
	return getResetPinWithPRSHint(hasPinResetService());
}


QString PinResetInformationModel::getResetPinWithPRSHint(bool pHasPRS) const
{
	return pHasPRS ?
	       //: ALL_PLATFORMS
		   tr("You may request a PIN Reset Letter with a new PIN and it's according activation code on the following website.") :
	       //This is required to hide the Hints if no PRS is available. Otherwise every caller has to do this.
		   QString();
}


QString PinResetInformationModel::getResetPinAtAuthorityHint() const
{
	//: ALL_PLATFORMS
	return tr("You may turn to the competent authority and reset the ID card PIN there.");
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
