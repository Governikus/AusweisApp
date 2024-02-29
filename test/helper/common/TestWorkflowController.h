/**
 * Copyright (c) 2021-2024 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "WorkflowRequest.h"
#include "controller/WorkflowController.h"

#include "TestWorkflowContext.h"

#include <QSharedPointer>

namespace governikus
{


class TestWorkflowController
	: public WorkflowController
{
	Q_OBJECT

	public:
		template<typename T = TestWorkflowContext, typename ... Args>
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(Args&& ... pArgs)
		{
			return WorkflowRequest::create<TestWorkflowController, T>(std::forward<Args>(pArgs) ...);
		}


		explicit TestWorkflowController(const QSharedPointer<WorkflowContext>& pContext);
};


} // namespace governikus
