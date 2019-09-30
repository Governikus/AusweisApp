/*!
 * \brief Controller of the whole program.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"
#include "EnumHelper.h"

#include <QAbstractNativeEventFilter>
#include <QSharedPointer>

class test_AppController;

namespace governikus
{

defineEnumType(Action,
		NONE,
		AUTH,
		SELF,
		PIN,
		READER_SETTINGS,
		REMOTE_SERVICE)


class WorkflowController;
class WorkflowRequest;
class CommandLineParser;

class AppController final
	: public QObject
	, public QAbstractNativeEventFilter
{
	Q_OBJECT

	private:
		friend class ::test_AppController;
		Q_DISABLE_COPY(AppController)

		friend class SignalHandler;
		friend class CommandLineParser;

		static bool cShowUi;
		Action mCurrentAction;
		QScopedPointer<WorkflowRequest> mWaitingRequest;
		QScopedPointer<WorkflowController> mActiveController;
		bool mShutdownRunning;
		const UIPlugIn* mUiDomination;
		bool mRestartApplication;

		bool canStartNewAction();
		void completeShutdown();

	public:
		AppController();
		virtual ~AppController() override;

		virtual bool eventFilter(QObject* pObj, QEvent* pEvent) override;
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult) override;

		bool start();

		bool shouldApplicationRestart() const;

	Q_SIGNALS:
		void fireStarted();
		void fireShutdown();
		void fireWorkflowStarted(QSharedPointer<WorkflowContext> pContext);
		void fireWorkflowFinished(QSharedPointer<WorkflowContext> pContext);
		void fireShowUi(UiModule pModule);
		void fireHideUi();
		void fireShowUserInformation(const QString& pInformationMessage);
		void fireShowReaderSettings();
#ifndef QT_NO_NETWORKPROXY
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
#endif
		void fireApplicationActivated();
		void fireUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted);
		void fireUiDominationReleased();

	private Q_SLOTS:
		void doShutdown();
		void onUiPlugin(UIPlugIn* pPlugin);
		void onWorkflowFinished();
		void onCloseReminderFinished(bool pDontRemindAgain);
		void onChangePinRequested();
		void onSelfAuthenticationRequested();
		void onAuthenticationRequest(const QSharedPointer<ActivationContext>& pActivationContext);
		void onRemoteServiceRequested();
		void onLanguageChanged();
		void onUILoaderShutdownComplete();
		void onUiDominationRequested(const UIPlugIn* pUi, const QString& pInformation);
		void onUiDominationRelease();
		void onRestartApplicationRequested();

	private:
		template<typename Controller, typename Context> bool startNewWorkflow(Action pAction, const QSharedPointer<Context>& pContext);

};

} // namespace governikus
