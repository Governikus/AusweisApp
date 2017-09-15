/*!
 * WorkflowController.h
 *
 * \brief Base class for controllers controlling a workflow (using a state machine).
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "states/AbstractGenericState.h"
#include "states/StateBuilder.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QStateMachine>

class test_ChangePinController;

namespace governikus
{

class WorkflowController
	: public QObject
{
	Q_OBJECT
	friend class ::test_ChangePinController;

	protected:
		QStateMachine mStateMachine;
		const QSharedPointer<WorkflowContext> mContext;

	public:
		WorkflowController(const QSharedPointer<WorkflowContext>& pContext);
		virtual ~WorkflowController();

		void run();

		QSharedPointer<WorkflowContext> getContext() const
		{
			return mContext;
		}


		template<typename T>
		T* addAndConnectState()
		{
			auto state = StateBuilder::createState<T>(mContext);
			mStateMachine.addState(state);
			return state;
		}


	Q_SIGNALS:
		void fireComplete();

};

} /* namespace governikus */
