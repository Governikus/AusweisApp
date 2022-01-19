/*!
 * \brief Mock implementation of WorkflowAuthenticateUi.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "view/WorkflowAuthenticateUi.h"

namespace governikus
{

class MockWorkflowAuthenticateUi
	: public WorkflowAuthenticateUi
{
	public:
		MockWorkflowAuthenticateUi(QSharedPointer<AuthModel> pModel)
			: WorkflowAuthenticateUi(pModel)
		{
		}


		virtual StepAdviseUserToRemoveCardUi* createStepAdviseUserToRemoveCardUi() override
		{
			return nullptr;
		}


		virtual StepAuthenticationDoneUi* createStepAuthenticationDoneUi() override
		{
			return nullptr;
		}


		virtual StepChooseDeviceUi* createStepChooseDeviceUi() override
		{
			return nullptr;
		}


		virtual StepChooseCardUi* createStepChooseCardUi() override
		{
			return nullptr;
		}


		virtual StepDidAuthenticateEac1Ui* createStepDidAuthenticateEac1Ui() override
		{
			return nullptr;
		}


		virtual StepErrorUi* createStepErrorUi() override
		{
			return nullptr;
		}


		virtual StepProcessingUi* createStepProcessingUi() override
		{
			return nullptr;
		}


};

} // namespace governikus
