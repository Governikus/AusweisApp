/**
 * Copyright (c) 2015-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Abstract layer to UI implementations.
 */

#pragma once

#include "EnumHelper.h"

#include <QNetworkProxy>
#include <QObject>

namespace governikus
{

class WorkflowRequest;
class WorkflowContext;

/*!
 * UI modules that can be requested to show.
 */
defineEnumType(UiModule,
		CURRENT,
		DEFAULT,
		IDENTIFY,
		SETTINGS,
		PINMANAGEMENT,
		HELP,
		SELF_AUTHENTICATION,

		// Desktop only
		TUTORIAL,
		UPDATEINFORMATION,

		// Mobile only
		REMOTE_SERVICE,
		CHECK_ID_CARD,
		SMART_EID
		)

class UIPlugIn
	: public QObject
{
	Q_OBJECT

	public:
		UIPlugIn() = default;
		~UIPlugIn() override = default;

		[[nodiscard]] virtual bool initialize();

	public Q_SLOTS:
		virtual void doShutdown() = 0;
		virtual void onWorkflowStarted(const QSharedPointer<WorkflowRequest>& pRequest) = 0;
		virtual void onWorkflowFinished(const QSharedPointer<WorkflowRequest>& pRequest) = 0;
		virtual void onWorkflowUnhandled(const QSharedPointer<WorkflowRequest>& pRequest);
		virtual void onApplicationInitialized();
		virtual void onApplicationStarted();
		virtual void onShowUi(UiModule pModule);
		virtual void onHideUi();
		virtual void onTranslationChanged();
		virtual void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
		virtual void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted);
		virtual void onUiDominationReleased();
		virtual void onShowUserInformation(const QString& pInformationMessage);

	Q_SIGNALS:
		void fireWorkflowRequested(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireShowUiRequested(UiModule pModule);
		void fireShowUserInformationRequested(const QString& pMsg);
		void fireQuitApplicationRequest(int pExitCode = EXIT_SUCCESS);
		void fireCloseReminderFinished(bool pDontRemindAgain);
		void fireApplicationActivated();

		void fireUiDominationRequest(const UIPlugIn* pUi, const QString& pInformation);
		void fireUiDominationRelease();
		void fireRestartApplicationRequested();
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::UIPlugIn, "governikus.UIPlugIn")
