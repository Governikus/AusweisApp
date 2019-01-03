/*!
 * \brief GUI for step "Error".
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "StepGui.h"

#include <QMessageBox>

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
		void closeActiveDialogs();

	public Q_SLOTS:
		virtual void forwardStep() override;

	private:
		QSharedPointer<WorkflowContext> mContext;
		AppQtMainWidget* const mMainWidget;
		QPointer<QMessageBox> mMessageBox;

	Q_SIGNALS:
		void switchedToPinSettings();
};

} // namespace governikus
