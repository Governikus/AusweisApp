/*!
 * \brief Controller for the authentication process.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
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
		explicit AuthController(QSharedPointer<AuthContext> pContext);
		virtual ~AuthController() = default;
};

} // namespace governikus
