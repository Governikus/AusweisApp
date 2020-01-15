/*!
 * \brief Qt widget based WorkflowAuthenticateUi implementation.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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
		void onStateChanged(const QString& pNewState);

	public:
		WorkflowAuthenticateQtGui(const QSharedPointer<AuthContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowAuthenticateQtGui() override;

		virtual void activate() override;
		virtual void deactivate() override;
		virtual bool verifyAbortWorkflow() override;

		virtual void onCloseActiveDialogs() override;
};

} // namespace governikus
