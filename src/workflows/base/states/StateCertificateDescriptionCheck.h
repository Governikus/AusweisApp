/*!
 * \copyright Copyright (c) 2014-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "context/AuthContext.h"
#include "GenericContextContainer.h"


namespace governikus
{

class StateCertificateDescriptionCheck
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	explicit StateCertificateDescriptionCheck(const QSharedPointer<WorkflowContext>& pContext);
	void run() override;

};

} // namespace governikus
