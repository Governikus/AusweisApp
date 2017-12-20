/*!
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

class test_StateCertificateDescriptionCheck;

namespace governikus
{

class StateCertificateDescriptionCheck
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateCertificateDescriptionCheck;

	StateCertificateDescriptionCheck(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;

};

} /* namespace governikus */
