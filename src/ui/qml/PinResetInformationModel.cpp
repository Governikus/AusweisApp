/*
 * \copyright Copyright (c) 2022 Governikus GmbH & Co. KG, Germany
 */

#include "PinResetInformationModel.h"

#include "AppSettings.h"
#include "ProviderConfiguration.h"

using namespace governikus;

PinResetInformationModel::PinResetInformationModel()
	: QObject()
{
	const auto& providerConfiguration = Env::getSingleton<ProviderConfiguration>();
	connect(providerConfiguration, &ProviderConfiguration::fireUpdated, this, &PinResetInformationModel::fireUpdated);

	const GeneralSettings& generalSettings = Env::getSingleton<AppSettings>()->getGeneralSettings();
	connect(&generalSettings, &GeneralSettings::fireLanguageChanged, this, &PinResetInformationModel::onTranslationChanged);
}


bool PinResetInformationModel::hasPinResetUrl() const
{
	return !getPinResetUrl().isEmpty();
}


QUrl PinResetInformationModel::getPinResetUrl() const
{
	const auto& info = Env::getSingleton<ProviderConfiguration>()->getProviderInfo(QStringLiteral("pinResetService"));
	return info.getHomepage();
}


QString PinResetInformationModel::getPinUnknownText() const
{
	//: LABEL ALL_PLATFORMS Explanation if the (Transport) PIN is unknown to the user on the Change PIN startpage.
	return tr("You cannot find your PIN letter? You have set a PIN when picking up the ID card or later that you cannot recall now?");
}


QString PinResetInformationModel::getPinUnknownHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Text in Hint to PRS if the (Transport) PIN is unknown to the user on the Change PIN startpage
			? tr("By using the PIN Reset Service you may request a new PIN.")
	       //: LABEL ALL_PLATFORMS Text in Hint when PRS is not available and the (Transport) PIN is unknown to the user on the Change PIN startpage
			: tr("If this is the case please turn to the competent authority and set a new PIN there.");
}


QString PinResetInformationModel::getNoPinAndNoPukHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
			? tr("If you forgot your PIN or do not have access to the PUK, you may request a new PIN here.")
			: QString();
}


QString PinResetInformationModel::getNoPinAndNoTransportPinHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
			? tr("If you know neither your Transport PIN nor your PIN, you may request a new PIN here.")
			: QString();
}


QString PinResetInformationModel::getPinForgottenHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
			? tr("If you cannot recall your PIN, you may request a new PIN here.")
			: QString();
}


QString PinResetInformationModel::getPinForgottenTutorialHint() const
{
	return hasPinResetUrl()
	       //: LABEL ANDROID IOS
			? tr("If you cannot recall your six-digit PIN or cannot find your PIN letter, may request a new PIN using the PIN Reset Service.")
	       //: LABEL ANDROID IOS
			: tr("You can always set a new PIN at the issuing authority if the (Transport) PIN is not known.");
}


QString PinResetInformationModel::getRequestNewPinHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
			? tr("You may request a new PIN here.")
			: QString();
}


QString PinResetInformationModel::getActivateOnlineFunctionHint() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the online identification funtion was not activated
			? tr("You can request activation of the online identification function here.")
			: QString();
}


QString PinResetInformationModel::getActivateOnlineFunctionDescriptionAndHint() const
{
	//: LABEL ANDROID IOS
	const auto& description = tr("The online identification function of your ID card is not yet activated.");

	if (!hasPinResetUrl())
	{
		return description;
	}

	return description + QStringLiteral("<br><br>") + getActivateOnlineFunctionHint();
}


QString PinResetInformationModel::getPinResetActionText() const
{
	return hasPinResetUrl()
	       //: LABEL ALL_PLATFORMS
			? tr("To provider")
			: QString();
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
