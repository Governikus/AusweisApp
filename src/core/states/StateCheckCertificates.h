/*!
 * \brief Checks whether the hashes of the certificates encountered so far
 * (i.e. those stored in the AuthContext) are in the CertificateDescription
 * extension of the eService certificate.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/AuthContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{

class StateCheckCertificates
	: public AbstractGenericState<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	StateCheckCertificates(const QSharedPointer<WorkflowContext>& pContext);
	virtual void run() override;
};

} /* namespace governikus */
