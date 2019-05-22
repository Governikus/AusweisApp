/*!
 * \copyright Copyright (c) 2015-2019 Governikus GmbH & Co. KG, Germany
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
	if (!mGui.isEnabled())
	{
		return;
	}

	pContext->setReaderPlugInTypes({ReaderManagerPlugInType::PCSC, ReaderManagerPlugInType::REMOTE});

	QSharedPointer<WorkflowGui> currentWorkflowGui;
	if (auto changePinContext = pContext.objectCast<ChangePinContext>())
	{
		currentWorkflowGui = mGui.createWorkflowChangePinUi(changePinContext);
		mGui.activateWorkflowUi(currentWorkflowGui);
		return;
	}

	bool allowHideAfterWorklow = true;
	if (auto selfAuthContext = pContext.objectCast<SelfAuthContext>())
	{
		if (mGui.askChangeTransportPinNow())
		{
			allowHideAfterWorklow = false;
			Q_EMIT pContext->fireCancelWorkflow();
		}
		currentWorkflowGui = mGui.createWorkflowSelfInfoUi(selfAuthContext);
	}
	else if (auto authContext = pContext.objectCast<AuthContext>())
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
	if (!mGui.isEnabled())
	{
		return;
	}

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


void UIPlugInWidgets::onHideUi()
{
	mGui.hideWithoutConfirmation();
}


#ifndef QT_NO_NETWORKPROXY
void UIPlugInWidgets::onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator)
{
	mGui.onProxyAuthenticationRequired(pProxy, pAuthenticator);
}


#endif


void UIPlugInWidgets::onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted)
{
	if (pUi == this)
	{
		return;
	}

	if (pAccepted)
	{
		mGui.setEnabled(false, pInformation);
	}
}


void UIPlugInWidgets::onUiDominationReleased()
{
	mGui.setEnabled(true);
}
