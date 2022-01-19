/*!
 * \brief Controller for the remote service process.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "controller/WorkflowController.h"

namespace governikus
{

class RemoteServiceContext;

class RemoteServiceController
	: public WorkflowController
{
	Q_OBJECT

	public:
		explicit RemoteServiceController(QSharedPointer<RemoteServiceContext> pContext);
		~RemoteServiceController() override;
};

} // namespace governikus
