/*!
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "GlobalStatus.h"

#include <QMessageBox>
#include <QSystemTrayIcon>


#ifndef QT_NO_NETWORKPROXY
class QNetworkProxy;
class QAuthenticator;
#endif

namespace governikus
{

class AppQtMainWidget;
class AuthContext;
class ChangePinContext;
class DiagnosisGui;
class SelfAuthContext;
class SetupAssistantGui;
class WorkflowGui;
class WorkflowAuthenticateQtGui;
class WorkflowChangePinQtGui;
class WorkflowSelfInfoQtGui;

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
		void switchToReaderSettings();

		void shutdown();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		void loadStyleSheet();
		void createTrayIcon();
		void closeDialogs();
		void hideFromTaskbar();
		void restoreToTaskbar();

	public Q_SLOTS:
		virtual void show(UiModule pModule = UiModule::CURRENT);
		virtual void onApplicationStarted();
		virtual void onShowUserInformation(const QString& pAppName);
#ifndef QT_NO_NETWORKPROXY
		void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
#endif

	private Q_SLOTS:
		void onActivated(QSystemTrayIcon::ActivationReason reason);
		void onCloseWindowRequested(bool* pDoClose);
		void onChangeHighContrast(bool* pHighContrastOn);
		void onSetupAssistantWizardRequest();
		void onDeveloperModeQuestion();
		void onDiagnosisRequested();
		void onAppUpdateReady(bool pSuccess, const GlobalStatus& pError);
		void onUpdateScheduled();
		void onCertificateRemoved(QString pDeviceName);
		void onSwitchToReaderSettingsRequested();

	private:
		AppQtMainWidget* mMainWidget;
		QIcon mIcon;
		QSystemTrayIcon* mTrayIcon;
		QSharedPointer<WorkflowGui> mActiveWorkflowUi;
		SetupAssistantGui* mSetupAssistantGui;
		DiagnosisGui* mDiagnosisGui;
		QMessageBox* mUpdateInfo;
		QMessageBox* mCertificateInfo;
		bool mAggressiveToForeground;

	Q_SIGNALS:
		void fireCloseReminderFinished(bool pDontRemindAgain);

		void fireChangePinRequested();
		void selfAuthenticationRequested();
		void fireSwitchToReaderSettingsRequested();
		void quitApplicationRequested();
};

} /* namespace governikus */
