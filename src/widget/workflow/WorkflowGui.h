/*!
 * \brief Base class for Qt based WorkflowUi implementations.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QScopedPointer>
#include <QWidget>

#include "AppQtMainWidget.h"
#include "context/WorkflowContext.h"
#include "step/StepGui.h"
#include "WorkflowQtWidget.h"
#include "WorkflowWidgetParent.h"

namespace governikus
{

class WorkflowQtWidget;

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


	Q_SIGNALS:
		void fireUserCancelled();
		void fireChangePinRequest();
		void fireSwitchToReaderSettingsRequested();

};

} /* namespace governikus */
