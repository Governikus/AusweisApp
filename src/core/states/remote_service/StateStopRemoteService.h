/*!
 * \brief Stop the remote service.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateStopRemoteService
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateStopRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;
	void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateStopRemoteService() override;

};

} /* namespace governikus */
