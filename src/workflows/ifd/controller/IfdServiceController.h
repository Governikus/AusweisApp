/*!
 * \brief Controller for the remote service process.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowRequest.h"
#include "controller/WorkflowController.h"

namespace governikus
{

class IfdServiceContext;

class IfdServiceController
	: public WorkflowController
{
	Q_OBJECT

	public:
		explicit IfdServiceController(QSharedPointer<IfdServiceContext> pContext);
		~IfdServiceController() override = default;
};

} // namespace governikus
