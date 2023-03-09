/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Base class for controllers controlling a workflow (using a state machine).
 */

#pragma once

#include "states/StateBuilder.h"

#include <QScopedPointer>
#include <QSharedPointer>
#include <QStateMachine>

namespace governikus
{

class WorkflowController
	: public QObject
{
	Q_OBJECT

	protected:
		QStateMachine mStateMachine;
		const QSharedPointer<WorkflowContext> mContext;

	public:
		explicit WorkflowController(const QSharedPointer<WorkflowContext>& pContext);

		void run();

		[[nodiscard]] Action getAction() const
		{
			return mContext->getAction();
		}


		[[nodiscard]] QSharedPointer<WorkflowContext> getContext() const
		{
			return mContext;
		}


		template<typename T>
		T* addState()
		{
			auto state = StateBuilder::createState<T>(mContext);
			mStateMachine.addState(state);
			return state;
		}

	Q_SIGNALS:
		void fireComplete();

};

} // namespace governikus
