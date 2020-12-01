/*!
 * \copyright Copyright (c) 2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"

namespace governikus
{

class TestWorkflowContext
	: public WorkflowContext
{
	Q_OBJECT

	public:
		TestWorkflowContext(bool pCanAllowed = false);
		virtual ~TestWorkflowContext() override = default;

		bool isCanAllowedMode() const override;
		void setCanAllowedMode(bool pCanAllowed);

	private:
		bool mCanAllowed;
};

} // namespace governikus
