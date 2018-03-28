/*!
 * \brief Start the remote service.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateStartRemoteService
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateStartRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;

	public:
		virtual ~StateStartRemoteService() override;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);

};

} /* namespace governikus */
