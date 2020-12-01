/*!
 * \copyright Copyright (c) 2018-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


namespace governikus
{

class StatePrepareChangePinRemote
	: public AbstractState
	, public GenericContextContainer<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StatePrepareChangePinRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	Q_SIGNALS:
		void fireEnterNewPacePin();
};

} // namespace governikus
