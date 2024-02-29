/**
 * Copyright (c) 2016-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Performs clean up of the ReaderManager,
 * e.g. disconnects all readers, clears the card connection, ...
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateCleanUpReaderManager
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
