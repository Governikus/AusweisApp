/**
 * Copyright (c) 2015-2025 Governikus GmbH & Co. KG, Germany
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
defineEnumTypeQmlExposed(UiModule,
		CURRENT,
		DEFAULT,
		IDENTIFY,
		SETTINGS,
		PINMANAGEMENT,
		HELP,
		SELF_AUTHENTICATION,
		ONBOARDING,

		// Desktop only
		UPDATEINFORMATION,

		// Mobile only
		REMOTE_SERVICE,
		CHECK_ID_CARD,
		SMART_EID
		)

class UiPlugin
	: public QObject
{
	Q_OBJECT

	public:
		UiPlugin() = default;
		~UiPlugin() override = default;

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
		virtual void onUiDomination(const UiPlugin* pUi, const QString& pInformation, bool pAccepted);
		virtual void onUiDominationReleased();
		virtual void onShowUserInformation(const QString& pInformationMessage);

	Q_SIGNALS:
		void fireWorkflowRequested(const QSharedPointer<WorkflowRequest>& pRequest);
		void fireShowUiRequested(UiModule pModule);
		void fireShowUserInformationRequested(const QString& pMsg);
		void fireQuitApplicationRequest(int pExitCode = EXIT_SUCCESS);
		void fireCloseReminderFinished(bool pDontRemindAgain);
		void fireApplicationActivated();
		void fireSystemSettingsChanged();

		void fireUiDominationRequest(const UiPlugin* pUi, const QString& pInformation);
		void fireUiDominationRelease();
		void fireRestartApplicationRequested();
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::UiPlugin, "governikus.UiPlugin")
