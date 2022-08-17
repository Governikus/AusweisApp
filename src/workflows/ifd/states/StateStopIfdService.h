/*!
 * \brief Stop the remote service.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateStopIfdService;


namespace governikus
{

class StateStopIfdService
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStopIfdService;

	private:
		explicit StateStopIfdService(const QSharedPointer<WorkflowContext>& pContext);

		void run() override;
		void onExit(QEvent* pEvent) override;

	public:
		~StateStopIfdService() override = default;

};

} // namespace governikus
