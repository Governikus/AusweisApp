/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the remote service process.
 */

#pragma once

#include "WorkflowRequest.h"
#include "controller/WorkflowController.h"

namespace governikus
{

class IfdServiceContext;

class IfdServiceController
	: public WorkflowController
{
	Q_OBJECT

	public:
		explicit IfdServiceController(QSharedPointer<IfdServiceContext> pContext);
		~IfdServiceController() override = default;
};

} // namespace governikus
