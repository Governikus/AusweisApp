/*!
 * \brief Controller for the self authentication process.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
		static QSharedPointer<WorkflowRequest> createWorkflowRequest();

		explicit SelfAuthController(QSharedPointer<SelfAuthContext> pContext);
		~SelfAuthController() override = default;
};

} // namespace governikus
