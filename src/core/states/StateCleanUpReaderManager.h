/*!
 * \brief Performs clean up of the ReaderManager,
 * e.g. disconnects all readers, clears the card connection, ...
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"

namespace governikus
{

class StateCleanUpReaderManager
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateCleanUpReaderManager(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
