/**
 * Copyright (c) 2014-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the PIN changing process.
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
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(bool pRequestTransportPin = false, bool pActivateUi = true);

		explicit ChangePinController(QSharedPointer<ChangePinContext> pContext);
		~ChangePinController() override = default;
};

} // namespace governikus
