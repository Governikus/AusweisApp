/*!
 * AppController.h
 *
 * \brief Controller of the whole program.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "ActivationHandler.h"
#include "EnumHelper.h"
#include "context/WorkflowContext.h"
#include "view/UIPlugIn.h"

#include <QSharedPointer>

namespace governikus
{

class WorkflowController;
class ReaderManagerPlugInInfo;

defineEnumType(Action,
		NONE, AUTH, SELF, PIN)

class AppController
	: public QObject
{
	Q_OBJECT

	private:
		Q_DISABLE_COPY(AppController)

		friend class SignalHandler;

		Action mCurrentAction;
		Action mWaitingAction;
		QScopedPointer<WorkflowController> mActiveController;

	public:
		AppController();
		virtual ~AppController();

		/*! Start the application controller.
		 *  \return Status: true on success, otherwise false
		 */
		bool start();

	Q_SIGNALS:
		void fireStarted();
		void fireShutdown();
		void fireWorkflowStarted(QSharedPointer<WorkflowContext> pContext);
		void fireWorkflowFinished(QSharedPointer<WorkflowContext> pContext);
		void fireShowUi(UiModule pModule);
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireShowUserInformation(const QString& pInformationMessage);

	private Q_SLOTS:
		void doShutdown();
		void onUiPlugin(UIPlugIn* pPlugin);
		void onActiveControllerDone();
		void onAppSettingsChanged();
		void onCloseReminderFinished(bool pDontRemindAgain);
		void onChangePinRequested();
		void onSelfAuthenticationRequested();
		void onAuthenticationRequest(ActivationContext* pActivationContext);

	private:
		template<typename Controller, typename Context> bool startNewWorkflow(Action pAction, QSharedPointer<Context> pContext);

};

} /* namespace governikus */
