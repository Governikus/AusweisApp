/**
 * Copyright (c) 2014-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Mock implementation of WorkflowAuthenticateUi.
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
