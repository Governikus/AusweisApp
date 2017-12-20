/*!
 * \brief Controller of the whole program.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "EnumHelper.h"

#include <QSharedPointer>

class QAuthenticator;
class QNetworkProxy;

namespace governikus
{

defineEnumType(Action,
		NONE,
		AUTH,
		SELF,
		PIN,
		READER_SETTINGS,
		REMOTE_SERVICE)


class UIPlugIn;
class WorkflowContext;
class WorkflowController;
class WorkflowRequest;


class AppController
	: public QObject
{
	Q_OBJECT

	private:
		Q_DISABLE_COPY(AppController)

		friend class SignalHandler;

		Action mCurrentAction;
		QScopedPointer<WorkflowRequest> mWaitingRequest;
		QScopedPointer<WorkflowController> mActiveController;

		bool canStartNewAction();

	public:
		AppController();
		virtual ~AppController();

		bool start();

	Q_SIGNALS:
		void fireStarted();
		void fireShutdown();
		void fireWorkflowStarted(QSharedPointer<WorkflowContext> pContext);
		void fireWorkflowFinished(QSharedPointer<WorkflowContext> pContext);
		void fireShowUi(UiModule pModule);
		void fireShowUserInformation(const QString& pInformationMessage);
		void fireShowReaderSettings();
#ifndef QT_NO_NETWORKPROXY
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
#endif

	private Q_SLOTS:
		void doShutdown();
		void onUiPlugin(UIPlugIn* pPlugin);
		void onWorkflowFinished();
		void onCloseReminderFinished(bool pDontRemindAgain);
		void onChangePinRequested();
		void onSwitchToReaderSettingsRequested();
		void onSelfAuthenticationRequested();
		void onAuthenticationRequest(const QSharedPointer<ActivationContext>& pActivationContext);
		void onRemoteServiceRequested();
		void onSettingsChanged();

	private:
		template<typename Controller, typename Context> bool startNewWorkflow(Action pAction, const QSharedPointer<Context>& pContext);

};

} /* namespace governikus */
