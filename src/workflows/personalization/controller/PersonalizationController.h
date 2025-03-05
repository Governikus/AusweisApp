/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowRequest.h"
#include "controller/WorkflowController.h"

namespace governikus
{

class PersonalizationContext;

class PersonalizationController
	: public WorkflowController
{
	Q_OBJECT

	public:
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(const QString& pAppletServiceUrl);

		explicit PersonalizationController(QSharedPointer<PersonalizationContext> pContext);
		~PersonalizationController() override = default;
};

} // namespace governikus
