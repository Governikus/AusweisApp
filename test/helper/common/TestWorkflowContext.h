/*!
 * \copyright Copyright (c) 2020-2021 Governikus GmbH & Co. KG, Germany
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
		~TestWorkflowContext() override = default;

		[[nodiscard]] bool isCanAllowedMode() const override;
		void setCanAllowedMode(bool pCanAllowed);

	private:
		bool mCanAllowed;
};

} // namespace governikus
