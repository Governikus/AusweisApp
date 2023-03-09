/**
 * Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the initialising a smartie.
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
