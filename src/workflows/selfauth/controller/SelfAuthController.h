/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the self authentication process.
 */

#pragma once

#include "WorkflowRequest.h"
#include "controller/WorkflowController.h"

namespace governikus
{

class SelfAuthContext;

class SelfAuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(bool pActivateUi = true);

		explicit SelfAuthController(QSharedPointer<SelfAuthContext> pContext);
		~SelfAuthController() override = default;
};

} // namespace governikus
