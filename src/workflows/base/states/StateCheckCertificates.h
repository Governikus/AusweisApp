/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Checks whether the hashes of the certificates encountered so far
 * (i.e. those stored in the AuthContext) are in the CertificateDescription
 * extension of the eService certificate.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"

namespace governikus
{

class StateCheckCertificates
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StateCheckCertificates(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
