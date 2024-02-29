/**
 * Copyright (c) 2022-2024 Governikus GmbH & Co. KG, Germany
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


bool PinResetInformationModel::hasPinResetService() const
{
	const auto& config = Env::getSingleton<ProviderConfiguration>();
	const auto& info = config->getProviderInfo(QStringLiteral("pinResetService"));
	return !info.getAddress().isEmpty();
}


QUrl PinResetInformationModel::getPinResetUrl() const
{
	const auto& config = Env::getSingleton<ProviderConfiguration>();
	const auto& info = config->getProviderInfo(QStringLiteral("pinResetService"));
	const auto& homepage = info.getAddress();

	if (homepage.isEmpty())
	{
		return QStringLiteral("https://www.behoerdenfinder.de");
	}

	if (LanguageLoader::getLocaleCode() != QStringLiteral("de"))
	{
		return homepage + QStringLiteral("/en");
	}

	return homepage;
}


QString PinResetInformationModel::getNoPinAndNoPukHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("You cannot use the PUK to reset your previously set card PIN. If you forgot your card PIN, you can use the PIN Reset Service to request a new PIN.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("If you don't have your Transport PIN letter and no access to the PUK, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
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
		   tr("You can request activation of the eID function.") :
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the eID function was not activated
		   tr("Please contact the competent authority to activate the eID function.") + authorityFinderSuffix();
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
		   tr("You can use the PIN Reset Service to request a new card PIN.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("You may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getPinResetHintTransportPin() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN, are not known.
		   tr("If you do not know either your Transport PIN or your card PIN, you can request a new PIN using the PIN Reset Service.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("If you know neither your Transport PIN nor your ID card PIN, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
}


QString PinResetInformationModel::getPinResetHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you have forgotten your ID card PIN, you can request a new PIN using the PIN Reset Service.") :
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you cannot recall your ID card PIN, you may turn to the competent authority and set a new ID card PIN there.") + authorityFinderSuffix();
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
	return QStringLiteral("<br/><br/>") + tr("To find your competent authority you may visit www.behoerdenfinder.de.");
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
