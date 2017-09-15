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
		bool mCanEntered;
		AuthenticateStepsWidget* mAuthenticateStepsWidget;
		QSharedPointer<StepAdviseUserToRemoveCardGui> mAdviseUserToRemoveCardGui;
		QSharedPointer<StepAuthenticationEac1Gui> mDidAuthenticateGui;
		QSharedPointer<StepChooseCardGui> mChooseCardGui;
		QSharedPointer<StepErrorGui> mErrorGui;
		QSharedPointer<StepProcessingGui> mProcessingGui;
		QSharedPointer<StepAuthenticationDoneGui> mAuthenticationDoneGui;

	private Q_SLOTS:
		void onForwardStep();
		void onCurrentStateChanged(const QString& pNewState);

	public:
		WorkflowAuthenticateQtGui(const QSharedPointer<AuthContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowAuthenticateQtGui();

		virtual void activate() override;
		virtual void deactivate() override;
		virtual bool verifyAbortWorkflow() override;
};

} /* namespace governikus */
