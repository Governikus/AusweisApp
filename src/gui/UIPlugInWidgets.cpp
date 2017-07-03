/*!
 * \copyright Copyright (c) 2015 Governikus GmbH & Co. KG
 */

#include "UIPlugInWidgets.h"

#include "workflow/WorkflowAuthenticateQtGui.h"
#include "workflow/WorkflowChangePinQtGui.h"
#include "workflow/WorkflowSelfInfoQtGui.h"

using namespace governikus;

UIPlugInWidgets::UIPlugInWidgets()
	: UIPlugIn()
	, mGui()
{
	connect(&mGui, &AppQtGui::quitApplicationRequested, this, &UIPlugIn::fireQuitApplicationRequest);
	connect(&mGui, &AppQtGui::fireChangePinRequested, this, &UIPlugIn::fireChangePinRequest);
	connect(&mGui, &AppQtGui::selfAuthenticationRequested, this, &UIPlugIn::fireSelfAuthenticationRequested);
	connect(&mGui, &AppQtGui::fireCloseReminderFinished, this, &UIPlugInWidgets::fireCloseReminderFinished);
	connect(this, &UIPlugIn::fireShowUserInformation, &mGui, &AppQtGui::onShowUserInformation);
	mGui.init();
}


UIPlugInWidgets::~UIPlugInWidgets()
{
}


void UIPlugInWidgets::doShutdown()
{
	mGui.shutdown();
}


void UIPlugInWidgets::onWorkflowStarted(QSharedPointer<WorkflowContext> pContext)
{
	pContext->setReaderType(ReaderManagerPlugInType::PCSC);

	QSharedPointer<WorkflowGui> currentWorkflowGui;
	if (auto changePinContext = pContext.dynamicCast<ChangePinContext>())
	{
		currentWorkflowGui = mGui.createWorkflowChangePinUi(changePinContext);
		mGui.activateWorkflowUi(currentWorkflowGui);
		return;
	}

	bool allowHideAfterWorklow = true;
	if (auto selfAuthContext = pContext.dynamicCast<SelfAuthenticationContext>())
	{
		if (mGui.askChangeTransportPinNow())
		{
			allowHideAfterWorklow = false;
			Q_EMIT pContext->fireCancelWorkflow();
		}
		currentWorkflowGui = mGui.createWorkflowSelfInfoUi(selfAuthContext);
	}
	else if (auto authContext = pContext.dynamicCast<AuthContext>())
	{
		if (mGui.askChangeTransportPinNow())
		{
			allowHideAfterWorklow = false;
			Q_EMIT pContext->fireCancelWorkflow();
		}
		currentWorkflowGui = mGui.createWorkflowAuthenticateUi(authContext);
	}

	Q_ASSERT(currentWorkflowGui != nullptr);
	mGui.activateWorkflowUi(currentWorkflowGui, allowHideAfterWorklow);
	pContext->setStateApproved();
}


void UIPlugInWidgets::onWorkflowFinished(QSharedPointer<WorkflowContext> pContext)
{
	Q_UNUSED(pContext)
	mGui.deactivateCurrentWorkflowUi();
}


void UIPlugInWidgets::onApplicationStarted()
{
	mGui.onApplicationStarted();
}


void UIPlugInWidgets::onShowUi(UiModule pModule)
{
	mGui.show(pModule);
}


void UIPlugInWidgets::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	mGui.onProxyAuthenticationRequired(pProxy, pAuthenticator);
}
