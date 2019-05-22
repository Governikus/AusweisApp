/*!
 * \brief Mock implementation of WorkflowAuthenticateUi.
 *
 * \copyright Copyright (c) 2014-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "view/WorkflowChangePinUi.h"

namespace governikus
{

class MockWorkflowChangePinUi
	: public WorkflowChangePinUi
{
	public:
		MockWorkflowChangePinUi(QSharedPointer<ChangePinModel> pModel)
			: WorkflowChangePinUi(pModel)
		{
		}


		virtual StepChangePinUi* createStepChangePinUi() override
		{
			return nullptr;
		}


		virtual StepChangePinDoneUi* createStepChangePinDoneUi() override
		{
			return nullptr;
		}


		virtual StepErrorUi* createStepErrorUi() override
		{
			return nullptr;
		}


};

} // namespace governikus
