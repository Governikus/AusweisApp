/*!
 * \brief GUI for step "Error".
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "StepGui.h"

namespace governikus
{

class AppQtMainWidget;

class StepErrorGui
	: public StepGui
{
	Q_OBJECT

	public:
		StepErrorGui(QSharedPointer<WorkflowContext> pContext, AppQtMainWidget* const pMainWidget);
		virtual ~StepErrorGui() override;

		virtual void reportError();

	public Q_SLOTS:
		virtual void forwardStep() override;

	private:
		QSharedPointer<WorkflowContext> mContext;
		AppQtMainWidget* const mMainWidget;

	Q_SIGNALS:
		void switchedToPinSettings();
};

} /* namespace governikus */
