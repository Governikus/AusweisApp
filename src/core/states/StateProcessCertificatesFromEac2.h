/*!
 * \brief State to process certificates from EAC2.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"


namespace governikus
{

class StateProcessCertificatesFromEac2
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateProcessCertificatesFromEac2;

	explicit StateProcessCertificatesFromEac2(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} // namespace governikus
