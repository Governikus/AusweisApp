/*!
 * \brief Controller for the remote service process.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowController.h"

namespace governikus
{

class RemoteServiceContext;

class RemoteServiceController
	: public WorkflowController
{
	Q_OBJECT

	public:
		RemoteServiceController(QSharedPointer<RemoteServiceContext> pContext);
		virtual ~RemoteServiceController();
};

} // namespace governikus
