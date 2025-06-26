/**
 * Copyright (c) 2022-2025 Governikus GmbH & Co. KG, Germany
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
		return QStringLiteral("https://servicesuche.bund.de");
	}
	return QStringLiteral("https://servicesuche.bund.de/#/en");
}


QUrl PinResetInformationModel::getPinResetActivationUrl() const
{
	return QStringLiteral("https://www.pin-ruecksetzbrief-bestellen.de/aktivierung");
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
		return homepage + QStringLiteral("/en");
	}

	return homepage;
}


QString PinResetInformationModel::getNoPinAndNoPukHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("If you don't have the letter with your Transport PIN and PUK, you can use the PIN Reset Service to request a new PIN.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("If you don't have the letter with your Transport PIN and PUK, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getRequestNewPinHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
		   tr("Request a new card PIN to be able to use the eID function again.") :
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
		   tr("You may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getActivateOnlineFunctionHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
		   tr("You may request the activation of the eID function.") :
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
		   tr("You may turn to the competent authority to activate the eID function.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getActivateOnlineFunctionDescription() const
{
	return GlobalStatus(GlobalStatus::Code::Card_Pin_Deactivated).toErrorDescription();
}


QString PinResetInformationModel::getActivateOnlineFunctionActionText() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS
		   tr("Go to Activation Service") :
	       //: LABEL ALL_PLATFORMS
		   tr("Find competent authority");
}


QString PinResetInformationModel::getPinResetHintNoPin() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN, are not known.
		   tr("Request a new card PIN to be able to use the eID function.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("You may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getPinResetHintTransportPin() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN, are not known.
		   tr("If you know neither your Transport PIN nor your ID card PIN, you can request a new PIN using the PIN Reset Service.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("If you know neither your Transport PIN nor your ID card PIN, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getPinResetHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you have forgotten your ID card PIN, you can request a new PIN using the PIN Reset Service.") :
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you don't know your ID card PIN, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getPinResetActionText() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS
		   tr("Go to PIN Reset Service") :
	       //: LABEL ALL_PLATFORMS
		   tr("Find competent authority");
}


QString PinResetInformationModel::authorityFinderSuffix() const
{
	auto serviceSearchPage = QStringLiteral("https://servicesuche.bund.de");
	if (LanguageLoader::getLocaleCode() != QLatin1String("de"))
	{
		serviceSearchPage += QStringLiteral("/#/en");
	}
	//: LABEL ALL_PLATFORMS %1 will be replaced with a link to a website.
	return QStringLiteral("<br/><br/>") + tr("To find your competent authority you may visit %1 .").arg(QStringLiteral("<a href=\"%1\">servicesuche.bund.de</a>").arg(serviceSearchPage));
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
