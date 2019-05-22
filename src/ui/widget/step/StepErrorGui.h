/*!
 * \brief GUI for step "Error".
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
		QString generateMailBody(const GlobalStatus& pStatus) const;

	Q_SIGNALS:
		void switchedToPinSettings();
};

} // namespace governikus
