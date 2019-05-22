/*!
 * \brief Base class for Qt based WorkflowUi implementations.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AppQtMainWidget.h"
#include "context/WorkflowContext.h"
#include "step/StepGui.h"
#include "WorkflowQtWidget.h"
#include "WorkflowWidgetParent.h"

#include <QScopedPointer>
#include <QWidget>

namespace governikus
{

class WorkflowGuiDelegate
	: public QObject
{
	Q_OBJECT

	public:
		WorkflowGuiDelegate();
};


class WorkflowGui
	: public QObject
{
	Q_OBJECT

	protected:
		QScopedPointer<WorkflowGuiDelegate> mDelegate;

	public:
		WorkflowGui();
		virtual ~WorkflowGui();

		WorkflowGuiDelegate* getWorkflowGuiDelegate() const
		{
			return mDelegate.data();
		}


		virtual bool verifyAbortWorkflow() = 0;

		virtual void activate()
		{
		}


		virtual void deactivate()
		{
		}


	public Q_SLOTS:
		virtual void onCloseActiveDialogs();

	Q_SIGNALS:
		void fireUserCancelled();
		void fireChangePinRequest();
		void fireCloseActiveDialogs();

};

} // namespace governikus
