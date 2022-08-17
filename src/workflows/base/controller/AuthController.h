/*!
 * \brief Controller for the authentication process.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "ActivationContext.h"
#include "WorkflowController.h"
#include "WorkflowRequest.h"

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
