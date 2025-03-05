/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
 */

#include "UiPlugin.h"

using namespace governikus;

void UiPlugin::onApplicationInitialized()
{
}


void UiPlugin::onApplicationStarted()
{
}


void UiPlugin::onShowUi(UiModule pModule)
{
	Q_UNUSED(pModule)
}


bool UiPlugin::initialize()
{
	return true;
}


void UiPlugin::onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest)
{
	Q_UNUSED(pRequest)
}


void UiPlugin::onHideUi()
{
}


void UiPlugin::onTranslationChanged()
{
}


void UiPlugin::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	Q_UNUSED(pProxy)
	Q_UNUSED(pAuthenticator)
}


void UiPlugin::onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted)
{
	Q_UNUSED(pUi)
	Q_UNUSED(pInformation)
	Q_UNUSED(pAccepted)
}


void UiPlugin::onUiDominationReleased()
{
}


void UiPlugin::onShowUserInformation(const QString& pInformationMessage)
{
	Q_UNUSED(pInformationMessage)
}
