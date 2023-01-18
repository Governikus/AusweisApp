/*!
 * \brief Controller for the PIN changing process.
 *
 * \copyright Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
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
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(bool pRequestTransportPin = false);

		explicit ChangePinController(QSharedPointer<ChangePinContext> pContext);
		~ChangePinController() override = default;
};

} // namespace governikus
