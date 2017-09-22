/*!
 * WorkflowAuthenticateQtGui.h
 *
 * \brief Qt widget based WorkflowAuthenticateUi implementation.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/AuthContext.h"
#include "GenericWorkflowGui.h"


namespace governikus
{

class AuthenticateStepsWidget;
class StepAdviseUserToRemoveCardGui;
class StepAuthenticationDoneGui;
class StepAuthenticationEac1Gui;
class StepChooseCardGui;
class StepErrorGui;
class StepProcessingGui;


class WorkflowAuthenticateQtGui
	: public GenericWorkflowGui<AuthContext>
{
	Q_OBJECT

	private:
		bool mRetryCounterUpdated;
		AuthenticateStepsWidget* mAuthenticateStepsWidget;
		QSharedPointer<StepAdviseUserToRemoveCardGui> mAdviseUserToRemoveCardGui;
		QSharedPointer<StepAuthenticationDoneGui> mAuthenticationDoneGui;
		QSharedPointer<StepAuthenticationEac1Gui> mDidAuthenticateGui;
		QSharedPointer<StepChooseCardGui> mChooseCardGui;
		QSharedPointer<StepErrorGui> mErrorGui;
		QSharedPointer<StepProcessingGui> mProcessingGui;

		void handleBlockedPin();

	private Q_SLOTS:
		void nextStep();
		void onStateChanged(const QString& pNewState);

	public:
		WorkflowAuthenticateQtGui(const QSharedPointer<AuthContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowAuthenticateQtGui();

		virtual void activate() override;
		virtual void deactivate() override;
		virtual bool verifyAbortWorkflow() override;
};

} /* namespace governikus */
