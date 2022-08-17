/*!
 * \copyright Copyright (c) 2021-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"

#include <QPair>

#include <functional>
#include <utility>

namespace governikus
{
defineEnumType(WorkflowControl
		, UNHANDLED
		, SKIP
		, ENQUEUE
		)

class WorkflowController;

class WorkflowRequest final
{
	Q_GADGET

	using BusyHandler = std::function<WorkflowControl (WorkflowRequest&, const QSharedPointer<WorkflowRequest>&, const QSharedPointer<WorkflowRequest>&)>;

	private:
		const std::function<QSharedPointer<WorkflowController>(const QSharedPointer<WorkflowContext>& pContext)> mGeneratorController;
		const std::function<QSharedPointer<WorkflowContext>()> mGeneratorContext;
		const BusyHandler mBusyHandler;

		QSharedPointer<WorkflowController> mController;
		QSharedPointer<WorkflowContext> mContext;

		template<typename Controller, typename Context, typename ... Args>
		static auto getGenerator(Args&& ... pArgs)
		{
			const auto& controller = [](const QSharedPointer<WorkflowContext>& pContext){
						return QSharedPointer<Controller>::create(pContext.staticCast<Context>());
					};

			const auto& context = [pArgs ...]() mutable {
						return QSharedPointer<Context>::create(std::forward<Args>(pArgs) ...);
					};

			return qMakePair(controller, context);
		}

	public:
		template<typename Controller, typename Context, typename Request = WorkflowRequest, typename ... Args>
		static QSharedPointer<WorkflowRequest> createWorkflowRequest(Args&& ... pArgs)
		{
			auto [controller, context] = getGenerator<Controller, Context, Args...>(std::forward<Args>(pArgs) ...);
			return QSharedPointer<Request>::create(controller, context);
		}


		template<typename Controller, typename Context, typename Request = WorkflowRequest, typename ... Args>
		static QSharedPointer<WorkflowRequest> createWorkflowRequestHandler(const BusyHandler& pBusyHandler, Args&& ... pArgs)
		{
			auto [controller, context] = getGenerator<Controller, Context, Args...>(std::forward<Args>(pArgs) ...);
			return QSharedPointer<Request>::create(controller, context, pBusyHandler);
		}


		WorkflowRequest(const std::function<QSharedPointer<WorkflowController>(const QSharedPointer<WorkflowContext>& pContext)>& pGeneratorController,
				const std::function<QSharedPointer<WorkflowContext>()>& pGeneratorContext,
				const BusyHandler& pBusyHandler = BusyHandler());

		void initialize();
		[[nodiscard]] bool isInitialized() const;
		[[nodiscard]] Action getAction() const;
		[[nodiscard]] QSharedPointer<WorkflowController> getController() const;
		[[nodiscard]] QSharedPointer<WorkflowContext> getContext() const;
		[[nodiscard]] WorkflowControl handleBusyWorkflow(const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow);
};

} // namespace governikus
