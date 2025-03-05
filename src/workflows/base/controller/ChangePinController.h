/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowController.h"
#include "WorkflowRequest.h"


namespace governikus
{

class ChangePinContext;

class ChangePinController
	: public WorkflowController
{
	Q_OBJECT

	public:
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(bool pRequestTransportPin = false, bool pActivateUi = true, bool pOnlyCheckPin = false);

		explicit ChangePinController(QSharedPointer<ChangePinContext> pContext);
		~ChangePinController() override = default;
};

} // namespace governikus
