/*!
 * \brief Start the remote service.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


namespace governikus
{

class StateStartRemoteService
	: public AbstractState
	, public GenericContextContainer<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateStartRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;

	public:
		virtual ~StateStartRemoteService() override;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);

};

} // namespace governikus
