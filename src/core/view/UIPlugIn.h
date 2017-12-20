/*!
 * \brief Abstract layer to UI implementations.
 *
 * \copyright Copyright (c) 2015-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationHandler.h"

#include <QNetworkProxy>
#include <QObject>

namespace governikus
{

class WorkflowContext;


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
		virtual void onShowReaderSettings();
		virtual void onSwitchToReaderSettingsRequested();
#ifndef QT_NO_NETWORKPROXY
		virtual void onProxyAuthenticationRequired(const QNetworkProxy& pProxy, QAuthenticator* pAuthenticator);
#endif

	Q_SIGNALS:
		void fireChangePinRequest();
		void fireSelfAuthenticationRequested();
		void fireSwitchToReaderSettingsRequested();
		void fireRemoteServiceRequested();
		void fireQuitApplicationRequest();
		void fireCloseReminderFinished(bool pDontRemindAgain);

		void fireShowUserInformation(const QString& pInformationMessage);
};

} /* namespace governikus */

Q_DECLARE_INTERFACE(governikus::UIPlugIn, "governikus.UIPlugIn")
