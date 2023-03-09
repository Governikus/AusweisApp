/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the authentication process.
 */

#pragma once

#include "WorkflowController.h"
#include "WorkflowRequest.h"
#include "context/ActivationContext.h"

namespace governikus
{

class AuthContext;

class AuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(const QSharedPointer<ActivationContext>& pActivationContext);
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(const QUrl& pUrl);

		explicit AuthController(QSharedPointer<AuthContext> pContext);
		~AuthController() override = default;
};

} // namespace governikus
