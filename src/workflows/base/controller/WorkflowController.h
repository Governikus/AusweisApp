/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Base class for controllers controlling a workflow (using a state machine).
 */

#pragma once

#include "context/WorkflowContext.h"
#include "states/StateBuilder.h"

#include <QSharedPointer>
#include <QStateMachine>

class test_AppController;

namespace governikus
{

class WorkflowController
	: public QObject
{
	Q_OBJECT
	friend class ::test_AppController;

	private:
		QStateMachine mStateMachine;
		const QSharedPointer<WorkflowContext> mContext;

	protected:
		template<typename T>
		[[nodiscard]] T* addState()
		{
			auto* state = StateBuilder::createState<T>(mContext);
			mStateMachine.addState(state);
			return state;
		}


		template<typename T>
		[[nodiscard]] T* addInitialState()
		{
			auto* state = addState<T>();
			mStateMachine.setInitialState(state);
			return state;
		}

	public:
		explicit WorkflowController(const QSharedPointer<WorkflowContext>& pContext);

		void run();

	Q_SIGNALS:
		void fireComplete();

};

} // namespace governikus
