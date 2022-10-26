/*!
 * \brief Controller of the whole program.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationController.h"
#include "EnumHelper.h"
#include "WorkflowRequest.h"

#include <QAbstractNativeEventFilter>
#include <QSharedPointer>

class test_AppController;

namespace governikus
{
class WorkflowController;
class CommandLineParser;

class AppController final
	: public QObject
	, public QAbstractNativeEventFilter
{
	Q_OBJECT
	Q_DISABLE_COPY(AppController)
	friend class ::test_AppController;
	friend int initApp(int&, char**);
	friend class CommandLineParser;

	private:
		static bool cShowUi;
		QSharedPointer<WorkflowRequest> mActiveWorkflow;
		QSharedPointer<WorkflowRequest> mWaitingRequest;
		ActivationController mActivationController;
		bool mShutdownRunning;
		const UIPlugIn* mUiDomination;
		bool mRestartApplication;
		int mExitCode;

		[[nodiscard]] bool canStartNewWorkflow() const;
		void completeShutdown();

	public:
		AppController();

		bool eventFilter(QObject* pObj, QEvent* pEvent) override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, qintptr* pResult) override;
#else
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, long* pResult) override;
#endif

		bool start();

		[[nodiscard]] bool shouldApplicationRestart() const;

	Q_SIGNALS:
		void fireInitialized();
		void fireStarted();
		void fireShutdown();
		void fireWorkflowStarted(QSharedPointer<WorkflowContext> pContext);
		void fireWorkflowFinished(QSharedPointer<WorkflowContext> pContext);
		void fireShowUi(UiModule pModule);
		void fireHideUi();
		void fireShowUserInformation(const QString& pInformationMessage);
		void fireTranslationChanged();
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireApplicationActivated();
		void fireUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted);
		void fireUiDominationReleased();

	private Q_SLOTS:
		void doShutdown(int pExitCode = EXIT_SUCCESS);
		void onUiPlugin(const UIPlugIn* pPlugin);
		void onWorkflowFinished();
		void onWorkflowRequested(const QSharedPointer<WorkflowRequest>& pRequest);
		void onCloseReminderFinished(bool pDontRemindAgain);
		void onAuthenticationContextRequest(const QSharedPointer<ActivationContext>& pActivationContext);
		void onLanguageChanged();
		void onUILoaderShutdownComplete();
		void onUiDominationRequested(const UIPlugIn* pUi, const QString& pInformation);
		void onUiDominationRelease();
		void onRestartApplicationRequested();

	private:
		bool startNewWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		static void clearCacheFolders();

};

} // namespace governikus
