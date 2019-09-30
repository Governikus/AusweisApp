/*!
 * \brief Controller for the self authentication process.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowController.h"

namespace governikus
{

class SelfAuthContext;

class SelfAuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		explicit SelfAuthController(QSharedPointer<SelfAuthContext> pContext);
		virtual ~SelfAuthController() = default;
};

} // namespace governikus
