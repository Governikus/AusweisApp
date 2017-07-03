/*!
 * \brief Controller for the step that tries to establish a PACE
 *        connection using the card's CAN.
 *
 * \copyright Copyright (c) 2016 Governikus GmbH & Co. KG
 */

#pragma once

#include "AbstractGenericState.h"
#include "context/WorkflowContext.h"


namespace governikus
{

class StateEstablishPaceCan
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateEstablishPaceCan(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
	virtual void onUserCancelled() override;

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireInvalidCan();
};

} /* namespace governikus */
