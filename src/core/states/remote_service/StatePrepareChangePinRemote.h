/*!
 * \copyright Copyright (c) 2018-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"


namespace governikus
{

class StatePrepareChangePinRemote
	: public AbstractGenericState<RemoteServiceContext>
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
