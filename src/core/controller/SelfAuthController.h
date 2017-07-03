/*!
 * SelfAuthController.h
 *
 * \brief Controller for the self authentication process.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "WorkflowController.h"

namespace governikus
{

class SelfAuthenticationContext;

class SelfAuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		SelfAuthController(QSharedPointer<SelfAuthenticationContext> pContext);
		virtual ~SelfAuthController();
};

} /* namespace governikus */
