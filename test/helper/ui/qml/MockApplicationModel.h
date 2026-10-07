/**
 * Copyright (c) 2026 Governikus Service GmbH, Germany
 */

#pragma once

#include "ApplicationModel.h"

namespace governikus
{

class MockApplicationModel
	: public ApplicationModel
{
	Q_OBJECT

	Q_PROPERTY(ApplicationModel::Workflow currentWorkflow MEMBER mCurrentWorkflow)

	public:
		MockApplicationModel();
		~MockApplicationModel() override;

	private:
		ApplicationModel::Workflow mCurrentWorkflow;
};

} // namespace governikus
