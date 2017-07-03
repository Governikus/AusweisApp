/*!
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "AppQtMainWidget.h"
#include "CredentialDialog.h"

#include "context/AuthContext.h"
#include "context/ChangePinContext.h"
#include "context/DiagnosisContext.h"
#include "context/ReaderDriverContext.h"
#include "context/SelfAuthenticationContext.h"

#include <QSystemTrayIcon>

namespace governikus
{

class WorkflowGui;
class WorkflowAuthenticateQtGui;
class WorkflowChangePinQtGui;
class WorkflowSelfInfoQtGui;
class SetupAssistantGui;
class DiagnosisGui;
class ReaderDriverGui;

class AppQtGui
	: public QObject
{
	Q_OBJECT

	public:
		AppQtGui();
		virtual ~AppQtGui();

		virtual void init();

		virtual QSharedPointer<WorkflowAuthenticateQtGui> createWorkflowAuthenticateUi(const QSharedPointer<AuthContext>& pContext);
		virtual QSharedPointer<WorkflowChangePinQtGui> createWorkflowChangePinUi(const QSharedPointer<ChangePinContext>& pContext);
		virtual QSharedPointer<WorkflowSelfInfoQtGui> createWorkflowSelfInfoUi(const QSharedPointer<SelfAuthenticationContext>& pContext);

		virtual void activateWorkflowUi(QSharedPointer<WorkflowGui> pWorkflowUi, bool pAllowHideAfterWorkflow = true);
		virtual void deactivateCurrentWorkflowUi();

		virtual bool askChangeTransportPinNow();

		void shutdown();

	protected:
		virtual bool eventFilter(QObject* pObject, QEvent* pEvent) override;

	private:
		void initGuiProfile();
		void loadStyleSheet(const QString& pStyleSheetName, bool pIsResource);
		QString readStyleSheet(const QString& pFileName);
		void createTrayIcon();
		void closeDialogs();
		void hideFromTaskbar();
		void restoreToTaskbar();

	public Q_SLOTS:
		virtual void show(UiModule pModule = UiModule::CURRENT);
		virtual void onApplicationStarted();
		virtual void onShowUserInformation(const QString& pAppName);
		void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);

	private Q_SLOTS:
		void onActivated(QSystemTrayIcon::ActivationReason reason);
		void onCloseWindowRequested(bool* pDoClose);
		void onDebugStyleSheetChanged(const QString& pPath);
		void onChangeHighContrast(bool* pHighContrastOn);
		void onSetupAssistantWizardRequest();
		void onDeveloperModeQuestion();
		void onReaderDriverRequested();
		void onDiagnosisRequested();

	private:
		AppQtMainWidget* mMainWidget;
		QSystemTrayIcon* mTrayIcon;
		QSharedPointer<WorkflowGui> mActiveWorkflowUi;
		SetupAssistantGui* mSetupAssistantGui;
		DiagnosisGui* mDiagnosisGui;
		ReaderDriverGui* mReaderDriverGui;
		bool mAggressiveToForeground;

	Q_SIGNALS:
		void fireCloseReminderFinished(bool pDontRemindAgain);

		void fireChangePinRequested();
		void selfAuthenticationRequested();
		void quitApplicationRequested();
};

} /* namespace governikus */
