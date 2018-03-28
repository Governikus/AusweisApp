/*!
 * \brief Controller for the step that tries to destroy an existing PACE connection.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

#include "context/ChangePinContext.h"

namespace governikus
{

class StateDestroyPace
	: public AbstractGenericState<ChangePinContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateDestroyPace(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	private Q_SLOTS:
		void onDestroyPaceDone(QSharedPointer<BaseCardCommand> pCommand);
};

} /* namespace governikus */
