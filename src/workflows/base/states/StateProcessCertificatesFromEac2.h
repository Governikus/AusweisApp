/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief State to process certificates from EAC2.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/AuthContext.h"


class test_StateProcessCertificatesFromEac2;


namespace governikus
{

class StateProcessCertificatesFromEac2
	: public AbstractState
	, public GenericContextContainer<AuthContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateProcessCertificatesFromEac2;

	private:
		explicit StateProcessCertificatesFromEac2(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;
};

} // namespace governikus
