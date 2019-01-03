/*!
 * \brief Qt widget based WorkflowSelfInfoUi implementation.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/SelfAuthContext.h"
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
	: public GenericWorkflowGui<SelfAuthContext>
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
		void onStateChanged(const QString& pNewState);

	public:
		WorkflowSelfInfoQtGui(const QSharedPointer<SelfAuthContext>& pContext, AppQtMainWidget* const pParentWidget);
		virtual ~WorkflowSelfInfoQtGui() override;

		virtual void activate() override;
		virtual void deactivate() override;
		virtual bool verifyAbortWorkflow() override;

		virtual void onCloseActiveDialogs() override;
};

} // namespace governikus
