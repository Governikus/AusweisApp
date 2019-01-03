/*!
 * \brief Controller for the step that tries to destroy an existing PACE connection.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/ChangePinContext.h"

class test_StateDestroyPace;

namespace governikus
{

class StateDestroyPace
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateDestroyPace;

	explicit StateDestroyPace(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onDestroyPaceDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
