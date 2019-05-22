/*!
 * \brief Stop the remote service.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

class test_StateStopRemoteService;

namespace governikus
{

class StateStopRemoteService
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStopRemoteService;

	explicit StateStopRemoteService(const QSharedPointer<WorkflowContext>& pContext);

	virtual void run() override;
	void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateStopRemoteService() override;

};

} // namespace governikus
