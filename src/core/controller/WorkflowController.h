/*!
 * \brief Base class for controllers controlling a workflow (using a state machine).
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

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
		T* addState()
		{
			auto state = StateBuilder::createState<T>(mContext);
			mStateMachine.addState(state);
			return state;
		}


	Q_SIGNALS:
		void fireComplete();

};

} /* namespace governikus */
