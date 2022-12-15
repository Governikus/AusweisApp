/*!
 * \copyright Copyright (c) 2015-2022 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugIn.h"

using namespace governikus;

void UIPlugIn::onApplicationInitialized()
{
}


void UIPlugIn::onApplicationStarted()
{
}


void UIPlugIn::onShowUi(UiModule pModule)
{
	Q_UNUSED(pModule)
}


bool UIPlugIn::initialize()
{
	return true;
}


void UIPlugIn::onHideUi()
{
}


void UIPlugIn::onTranslationChanged()
{
}


void UIPlugIn::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	Q_UNUSED(pProxy)
	Q_UNUSED(pAuthenticator)
}


void UIPlugIn::onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted)
{
	Q_UNUSED(pUi)
	Q_UNUSED(pInformation)
	Q_UNUSED(pAccepted)
}


void UIPlugIn::onUiDominationReleased()
{
}


void UIPlugIn::onShowUserInformation(const QString& pInformationMessage)
{
	Q_UNUSED(pInformationMessage)
}
