/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
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


void UIPlugIn::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	Q_UNUSED(pProxy)
	Q_UNUSED(pAuthenticator)
}
