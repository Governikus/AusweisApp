/*!
 * \brief Generic base class for Qt based WorkflowUi implementations.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppQtMainWidget.h"
#include "context/WorkflowContext.h"
#include "step/StepGui.h"
#include "WorkflowGui.h"
#include "WorkflowQtWidget.h"

#include <QSharedPointer>
#include <QWidget>


namespace governikus
{

template<typename CONTEXT>
class GenericWorkflowGui
	: public WorkflowGui
{
	protected:
		AppQtMainWidget* mParentWidget;
		WorkflowQtWidget* mWidget;
		QSharedPointer<StepGui> mStepGui;
		QSharedPointer<CONTEXT> mContext;

	public:
		GenericWorkflowGui(const QSharedPointer<WorkflowContext>& pContext, AppQtMainWidget* pParentWidget, WorkflowQtWidget* pWidget)
			: WorkflowGui()
			, mParentWidget(pParentWidget)
			, mWidget(pWidget)
			, mStepGui(nullptr)
			, mContext(pContext.objectCast<CONTEXT>())
		{
			Q_ASSERT(mContext != nullptr);
			connect(this, &WorkflowGui::fireUserCancelled, mContext.data(), &WorkflowContext::fireCancelWorkflow);
		}


		virtual void deactivate() override
		{
			deactivateCurrentStepUi();
		}


		virtual void activateStepUi(const QSharedPointer<StepGui>& pStepUi)
		{
			Q_ASSERT(pStepUi);
			if (mStepGui == pStepUi)
			{
				return;
			}

			deactivateCurrentStepUi();

			mStepGui = pStepUi;
			if (mWidget != nullptr)
			{
				QObject::connect(mStepGui->getStepGuiDelegate(), &StepGuiDelegate::setForwardButtonState, mWidget, &WorkflowQtWidget::setForwardButtonState);
				QObject::connect(mStepGui->getStepGuiDelegate(), &StepGuiDelegate::setCancelButtonState, mWidget, &WorkflowQtWidget::setCancelButtonState);
			}
			pStepUi->activate();
		}


	private:
		void deactivateCurrentStepUi()
		{
			if (mStepGui == nullptr)
			{
				return;
			}

			mStepGui->deactivate();
			if (mWidget != nullptr)
			{
				QObject::disconnect(mStepGui->getStepGuiDelegate(), &StepGuiDelegate::setForwardButtonState, mWidget, &WorkflowQtWidget::setForwardButtonState);
				QObject::disconnect(mStepGui->getStepGuiDelegate(), &StepGuiDelegate::setCancelButtonState, mWidget, &WorkflowQtWidget::setCancelButtonState);
			}
			mStepGui.clear();
		}


};

} // namespace governikus
