/*!
 * \copyright Copyright (c) 2015-2018 Governikus GmbH & Co. KG, Germany
 */

#include "UIPlugIn.h"

using namespace governikus;

UIPlugIn::UIPlugIn()
{
}


UIPlugIn::~UIPlugIn()
{
}


void UIPlugIn::onApplicationStarted()
{

}


void UIPlugIn::onShowUi(UiModule pModule)
{
	Q_UNUSED(pModule)
}


void UIPlugIn::onShowReaderSettings()
{
}


void UIPlugIn::onSwitchToReaderSettingsRequested()
{
	Q_EMIT fireSwitchToReaderSettingsRequested();
}


#ifndef QT_NO_NETWORKPROXY
void UIPlugIn::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	Q_UNUSED(pProxy)
	Q_UNUSED(pAuthenticator)
}


#endif
