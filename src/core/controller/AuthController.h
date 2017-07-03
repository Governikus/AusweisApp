/*!
 * AuthController.h
 *
 * \brief Controller for the authentication process.
 *
 * \copyright Copyright (c) 2014 Governikus GmbH & Co. KG
 */

#pragma once

#include "WorkflowController.h"

namespace governikus
{

class AuthContext;

class AuthController
	: public WorkflowController
{
	Q_OBJECT

	public:
		AuthController(QSharedPointer<AuthContext> pContext);
		virtual ~AuthController();
};

} /* namespace governikus */
