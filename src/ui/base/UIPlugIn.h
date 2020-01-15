/*!
 * \brief Abstract layer to UI implementations.
 *
 * \copyright Copyright (c) 2015-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "EnumHelper.h"

#include <QNetworkProxy>
#include <QObject>

namespace governikus
{

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
		UPDATEINFORMATION
		)

class UIPlugIn
	: public QObject
{
	Q_OBJECT

	public:
		UIPlugIn();
		virtual ~UIPlugIn();

	public Q_SLOTS:
		virtual void doShutdown() = 0;
		virtual void onWorkflowStarted(QSharedPointer<WorkflowContext> pContext) = 0;
		virtual void onWorkflowFinished(QSharedPointer<WorkflowContext> pContext) = 0;
		virtual void onApplicationStarted();
		virtual void onShowUi(UiModule pModule);
		virtual void onHideUi();
		virtual void onShowReaderSettings();
#ifndef QT_NO_NETWORKPROXY
		virtual void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
#endif
		virtual void onUiDomination(const UIPlugIn* pUi, const QString& pInformation, bool pAccepted);
		virtual void onUiDominationReleased();

	Q_SIGNALS:
		void fireChangePinRequest();
		void fireSelfAuthenticationRequested();
		void fireRemoteServiceRequested();
		void fireQuitApplicationRequest();
		void fireCloseReminderFinished(bool pDontRemindAgain);
		void fireApplicationActivated();

		void fireShowUserInformation(const QString& pInformationMessage);
		void fireUiDominationRequest(const UIPlugIn* pUi, const QString& pInformation);
		void fireUiDominationRelease();
		void fireRestartApplicationRequested();
};

} // namespace governikus

Q_DECLARE_INTERFACE(governikus::UIPlugIn, "governikus.UIPlugIn")
