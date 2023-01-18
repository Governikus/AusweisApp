/*!
 * \brief Start the remote service.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateStartIfdService;


namespace governikus
{

class StateStartIfdService
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateStartIfdService;

	private:
		explicit StateStartIfdService(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	public:
		~StateStartIfdService() override = default;
};

} // namespace governikus
