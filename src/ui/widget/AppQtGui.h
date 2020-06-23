/*!
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "DiagnosisGui.h"
#include "GlobalStatus.h"
#include "SetupAssistantGui.h"
#include "TrayIcon.h"
#include "UpdateWindow.h"
#include "workflow/WorkflowAuthenticateQtGui.h"
#include "workflow/WorkflowChangePinQtGui.h"
#include "workflow/WorkflowSelfInfoQtGui.h"

#include <QMessageBox>


namespace governikus
{

class AppQtGui
	: public QObject
{
	Q_OBJECT

	public:
		AppQtGui();
		virtual ~AppQtGui() override;

		virtual void init();

		virtual QSharedPointer<WorkflowAuthenticateQtGui> createWorkflowAuthenticateUi(const QSharedPointer<AuthContext>& pContext);
		virtual QSharedPointer<WorkflowChangePinQtGui> createWorkflowChangePinUi(const QSharedPointer<ChangePinContext>& pContext);
		virtual QSharedPointer<WorkflowSelfInfoQtGui> createWorkflowSelfInfoUi(const QSharedPointer<SelfAuthContext>& pContext);

		virtual void activateWorkflowUi(QSharedPointer<WorkflowGui> pWorkflowUi, bool pAllowHideAfterWorkflow = true);
		virtual void deactivateCurrentWorkflowUi();

		virtual bool askChangeTransportPinNow();

		void shutdown();
		void hideWithoutConfirmation();
		void setEnabled(bool pEnable, const QString& pLockedInfoText = QString());
		bool isEnabled() const;

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		void loadStyleSheet();
		void closeDialogs();

	public Q_SLOTS:
		virtual void show(UiModule pModule = UiModule::CURRENT);
		virtual void onApplicationStarted();
		virtual void onShowUserInformation(const QString& pAppName);
		void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);

	private Q_SLOTS:
		void onCloseWindowRequested(bool* pDoClose);
		void onCloseActiveDialogs();
		void onChangeHighContrast(bool* pHighContrastOn);
		void onSetupAssistantWizardRequest();
		void onDeveloperModeQuestion();
		void onDiagnosisRequested();
		void onAppUpdateReady(bool pSuccess, const GlobalStatus& pError);
		void onUpdateScheduled();
		void onCertificateRemoved(const QString& pDeviceName);
		void onSwitchUiRequested();

	private:
		AppQtMainWidget* mMainWidget;
		TrayIcon mTrayIcon;
		QSharedPointer<WorkflowGui> mActiveWorkflowUi;
		SetupAssistantGui* mSetupAssistantGui;
		DiagnosisGui* mDiagnosisGui;
		QMessageBox* mUpdateInfo;
		QMessageBox* mCertificateInfo;
		QMessageBox* mLockedInfo;
		UpdateWindow* mUpdateWindow;
		bool mAggressiveToForeground;

	Q_SIGNALS:
		void fireCloseReminderFinished(bool pDontRemindAgain);

		void fireChangePinRequested();
		void selfAuthenticationRequested();
		void quitApplicationRequested();
		void fireRestartApplicationRequested();
		void fireCloseActiveDialogs();
};

} // namespace governikus
