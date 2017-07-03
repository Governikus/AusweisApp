/*!
 * StepAdviseUserToRemoveCardGui.h
 *
 * \brief Qt UI for the advise user to remove card step.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "StepGui.h"
#include "context/AuthContext.h"

#include <QMessageBox>
#include <QSharedPointer>

namespace governikus
{

class StepAdviseUserToRemoveCardGui
	: public StepGui
{
	Q_OBJECT

	public Q_SLOTS:
		void onReaderManagerSignal();

	public:
		StepAdviseUserToRemoveCardGui(QSharedPointer<WorkflowContext> pContext, QWidget* const pMainWidget);
		virtual ~StepAdviseUserToRemoveCardGui();

		virtual void activate();

	private:
		QSharedPointer<WorkflowContext> mContext;
		QWidget* const mMainWidget;
		QMessageBox* mMessageBox;
};

} /* namespace governikus */
