/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateCertificateDescriptionCheck;


namespace governikus
{

class StateCertificateDescriptionCheck
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateCertificateDescriptionCheck;

	private:
		explicit StateCertificateDescriptionCheck(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

};

} // namespace governikus
