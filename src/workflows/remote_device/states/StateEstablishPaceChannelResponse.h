/*!
 * \brief This state executes the remote message PACE channel establish.
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

namespace governikus
{

class StateEstablishPaceChannelResponse
	: public AbstractState
	, public GenericContextContainer<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateEstablishPaceChannelResponse(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	Q_SIGNALS:
		void fireWrongPacePassword();
};

} // namespace governikus
