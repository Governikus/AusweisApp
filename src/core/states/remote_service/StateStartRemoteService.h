/*!
 * \brief Start the remote service.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

class test_StateStartRemoteService;

namespace governikus
{

class StateStartRemoteService
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStartRemoteService;

	explicit StateStartRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;

	public:
		virtual ~StateStartRemoteService() override;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);

};

} // namespace governikus
