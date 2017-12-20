/*!
 * \brief QWidgets implementation of UIPlugIn.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppQtGui.h"
#include "view/UIPlugIn.h"

namespace governikus
{

class AppQtGui;

class UIPlugInWidgets
	: public UIPlugIn
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID "governikus.UIPlugIn" FILE "metadata.json")
	Q_INTERFACES(governikus::UIPlugIn)

	private:
		AppQtGui mGui;

	public:
		UIPlugInWidgets();
		virtual ~UIPlugInWidgets() override;

	public Q_SLOTS:
		virtual void doShutdown() override;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) override;
		virtual void onApplicationStarted() override;
		virtual void onShowUi(UiModule pModule) override;
		virtual void onShowReaderSettings() override;
#ifndef QT_NO_NETWORKPROXY
		virtual void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator) override;
#endif
};

} /* namespace governikus */
