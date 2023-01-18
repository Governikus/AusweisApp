/*
 * \copyright Copyright (c) 2022-2023 Governikus GmbH & Co. KG, Germany
 */

#include "PinResetInformationModel.h"

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
		return tr("https://www.personalausweisportal.de/EN");
	}

	return homepage;
}


QString PinResetInformationModel::getPinUnknownText() const
{
	//: LABEL ALL_PLATFORMS Explanation if the (Transport) PIN is unknown to the user on the Change PIN startpage.
	return tr("You cannot find your PIN letter? You have set a ID card PIN when picking up the ID card or later that you cannot recall now?");
}


QString PinResetInformationModel::getPinUnknownHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Text in Hint to PRS if the (Transport) PIN is unknown to the user on the Change PIN startpage
		   tr("By using the PIN Reset Service you may request a new ID card PIN.") :
	       //: LABEL ALL_PLATFORMS Text in Hint when PRS is not available and the (Transport) PIN is unknown to the user on the Change PIN startpage
		   tr("If this is the case please turn to the competent authority and set a new ID card PIN there.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getNoPinAndNoPukHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("If you have forgotten your ID card PIN or do not have access to the PUK, you may request a new PIN here.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested PUK but both, PUK and PIN are not known.
		   tr("If you have forgotten your ID card PIN or do not have access to the PUK, you may turn to the competent authority and set a new PIN there.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getNoPinAndNoTransportPinHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("If you know neither your Transport PIN nor your ID card PIN, you may request a new PIN here.") :
	       //: LABEL ALL_PLATFORMS Hint text for requested Transport PIN but both, Transport PIN and PIN are not known.
		   tr("If you know neither your Transport PIN nor your ID card PIN, you may turn to the competent authority and set a new PIN there.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getPinForgottenHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you cannot recall your ID card PIN, you may request a new PIN here.") :
	       //: LABEL ALL_PLATFORMS Hint text for PIN but it is unknown.
		   tr("If you cannot recall your ID card PIN, you may turn to the competent authority and set a new PIN there.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getPinForgottenTutorialHint() const
{
	return hasPinResetService() ?
	       //: LABEL ANDROID IOS
		   tr("If you cannot recall your six-digit PIN or cannot find your PIN letter, you may request a new PIN using the PIN Reset Service.") :
	       //: LABEL ANDROID IOS
		   tr("You can always set a new ID card PIN at the competent authority if the (Transport) PIN is not known.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getRequestNewPinHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
		   tr("You may request a new ID card PIN here.") :
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because of a blocked PUK
		   tr("You may turn to the competent authority and set a new ID card PIN there.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getActivateOnlineFunctionHint() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the online identification function was not activated
		   tr("You can request activation of the online identification function here.") :
	       //: LABEL ALL_PLATFORMS Hint when a workflow failed because the online identification function was not activated
		   tr("Please contact the competent authority to activate the online identification function.<br/><br/>For further information, please visit the ID card portal.");
}


QString PinResetInformationModel::getActivateOnlineFunctionDescription() const
{
	return GlobalStatus(GlobalStatus::Code::Card_Pin_Deactivated).toErrorDescription();
}


QString PinResetInformationModel::getPinResetActionText() const
{
	return hasPinResetService() ?
	       //: LABEL ALL_PLATFORMS
		   tr("To provider") :
	       //: LABEL ALL_PLATFORMS
		   tr("Open website");
}


void PinResetInformationModel::onTranslationChanged()
{
	Q_EMIT fireUpdated();
}
