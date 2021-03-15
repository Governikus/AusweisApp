/*!
 * \brief Controller of the whole program.
 *
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationController.h"
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
		ActivationController mActivationController;
		bool mShutdownRunning;
		const UIPlugIn* mUiDomination;
		bool mRestartApplication;
		int mExitCode;

		[[nodiscard]] bool canStartNewAction() const;
		void completeShutdown();

	public:
		AppController();
		~AppController() override;

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
		void fireShowReaderSettings();
		void fireTranslationChanged();
		void fireProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		void fireApplicationActivated();
		void fireUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted);
		void fireUiDominationReleased();

	private Q_SLOTS:
		void doShutdown();
		void doShutdown(int pExitCode);
		void onUiPlugin(UIPlugIn* pPlugin);
		void onWorkflowFinished();
		void onCloseReminderFinished(bool pDontRemindAgain);
		void onChangePinRequested(bool pRequestTransportPin);
		void onSelfAuthenticationRequested();
		void onAuthenticationRequest(const QUrl& pUrl);
		void onAuthenticationContextRequest(const QSharedPointer<ActivationContext>& pActivationContext);
		void onRemoteServiceRequested();
		void onLanguageChanged();
		void onUILoaderShutdownComplete();
		void onUiDominationRequested(const UIPlugIn* pUi, const QString& pInformation);
		void onUiDominationRelease();
		void onRestartApplicationRequested();

	private:
		template<typename Controller, typename Context> bool startNewWorkflow(Action pAction, const QSharedPointer<Context>& pContext);
		static void clearCacheFolders();

};

} // namespace governikus
