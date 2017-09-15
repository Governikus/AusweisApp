/*!
 * WorkflowSelfInfoQtGui.h
 *
 * \brief Qt widget based WorkflowSelfInfoUi implementation.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "context/SelfAuthenticationContext.h"
#include "GenericWorkflowGui.h"

namespace governikus
{

class AuthenticateStepsWidget;
class StepAdviseUserToRemoveCardGui;
class StepShowSelfAuthenticationDataGui;
class StepAuthenticationEac1Gui;
class StepChooseCardGui;
class StepErrorGui;
class StepProcessingGui;

class WorkflowSelfInfoQtGui
	: public GenericWorkflowGui<SelfAuthenticationContext>
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
		QSharedPointer<StepShowSelfAuthenticationDataGui> mShowSelfAuthenticationDataGui;

	private Q_SLOTS:
		void onForwardStep();
		void onCurrentStateChanged(const QString& pNewState);

	public:
		WorkflowSelfInfoQtGui(const QSharedPointer<SelfAuthenticationContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowSelfInfoQtGui();

		virtual void activate() override;
		virtual void deactivate() override;
		virtual bool verifyAbortWorkflow() override;
};

} /* namespace governikus */
