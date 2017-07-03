/*!
 * WorkflowSelfInfoQtGui.h
 *
 * \brief Qt widget based WorkflowSelfInfoUi implementation.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "GenericWorkflowGui.h"
#include "context/SelfAuthenticationContext.h"

namespace governikus
{

class AppQtMainWidget;
class AuthenticateStepsWidget;
class StepGui;
class WorkflowQtWidget;
class StepAdviseUserToRemoveCardGui;
class StepChooseCardGui;
class StepChooseDeviceGui;
class StepAuthenticationEac1Gui;
class StepErrorGui;
class StepProcessingGui;
class StepShowSelfAuthenticationDataGui;
class StepTransportPinReminderGui;

class WorkflowSelfInfoQtGui
	: public GenericWorkflowGui<SelfAuthenticationContext>
{
	Q_OBJECT

	private:
		bool mRetryCounterUpdated;
		AuthenticateStepsWidget* mAuthenticateStepsWidget;
		QSharedPointer<StepAdviseUserToRemoveCardGui> mStepAdviseUserToRemoveCardGui;
		QSharedPointer<StepChooseCardGui> mStepChooseCardGui;
		QSharedPointer<StepAuthenticationEac1Gui> mStepAuthenticationEac1Gui;
		QSharedPointer<StepErrorGui> mStepErrorGui;
		QSharedPointer<StepProcessingGui> mStepProcessingGui;
		QSharedPointer<StepShowSelfAuthenticationDataGui> mStepShowSelfAuthenticationDataGui;

		bool showPinBlockageDialog();

	private Q_SLOTS:
		void onForwardStep();

	public:
		WorkflowSelfInfoQtGui(const QSharedPointer<SelfAuthenticationContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowSelfInfoQtGui();

		virtual void activate() override;
		virtual void deactivate() override;

		virtual bool verifyAbortWorkflow() override;

		const QSharedPointer<SelfAuthenticationContext>& getContext();

	public Q_SLOTS:
		void onCurrentStateChanged(const QString& pNewState);
};

} /* namespace governikus */
