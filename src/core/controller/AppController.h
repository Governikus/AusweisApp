/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller of the whole program.
 */

#pragma once

#include "EnumHelper.h"
#include "UIPlugIn.h"
#include "WorkflowRequest.h"

#include <QAbstractNativeEventFilter>
#include <QAuthenticator>
#include <QNetworkProxy>
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
		bool mShutdownRunning;
		const UIPlugIn* mUiDomination;
		bool mRestartApplication;
		int mExitCode;

		[[nodiscard]] bool canStartNewWorkflow() const;
		void completeShutdown();
		void waitForNetworkConnections(const std::function<void()>& pExitFunc);

	public:
		AppController();

		bool eventFilter(QObject* pObj, QEvent* pEvent) override;
		bool nativeEventFilter(const QByteArray& pEventType, void* pMessage, qintptr* pResult) override;

		void start();

		[[nodiscard]] bool shouldApplicationRestart() const;

	Q_SIGNALS:
		void fireInitialized();
		void fireStarted();
		void fireShutdown();
		void fireWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest);
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
		void onLanguageChanged();
		void onUiDominationRequested(const UIPlugIn* pUi, const QString& pInformation);
		void onUiDominationRelease();
		void onRestartApplicationRequested();

	private:
		bool startNewWorkflow(const QSharedPointer<WorkflowRequest>& pRequest);
		static void clearCacheFolders();

};

} // namespace governikus
