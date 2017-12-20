/*!
 * \brief Controller for the PIN changing process.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowController.h"


namespace governikus
{

class ChangePinContext;

class ChangePinController
	: public WorkflowController
{
	Q_OBJECT

	public:
		ChangePinController(QSharedPointer<ChangePinContext> pContext);
		virtual ~ChangePinController();
};

} /* namespace governikus */
