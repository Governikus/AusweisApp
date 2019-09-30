/*!
 * \brief Stop the remote service.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


namespace governikus
{

class StateStopRemoteService
	: public AbstractState
	, public GenericContextContainer<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateStopRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;
	void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateStopRemoteService() override;

};

} // namespace governikus
