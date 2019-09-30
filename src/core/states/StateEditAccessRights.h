/*!
 *
 * \brief This state waits for editing of the effective CHAT
 *
 * \copyright Copyright (c) 2016-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"

namespace governikus
{

class StateEditAccessRights
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateEditAccessRights(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

	void printRights(const QString& pTitle, const QSet<AccessRight>& pRights) const;
};

} // namespace governikus
