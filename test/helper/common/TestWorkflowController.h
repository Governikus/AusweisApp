/*!
 * \copyright Copyright (c) 2021-2023 Governikus GmbH & Co. KG, Germany
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
		template<typename ... Args>
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(Args&& ... pArgs)
		{
			return WorkflowRequest::createWorkflowRequest<TestWorkflowController, TestWorkflowContext>(std::forward<Args>(pArgs) ...);
		}


		explicit TestWorkflowController(const QSharedPointer<TestWorkflowContext>& pContext);
};


} // namespace governikus
