/**
 * Copyright (c) 2021-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"

#include <QPair>
#include <QVariant>

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

	using BusyHandler = std::function<WorkflowControl (const QSharedPointer<WorkflowRequest>&, const QSharedPointer<WorkflowRequest>&)>;

	private:
		const std::function<QSharedPointer<WorkflowController>(const QSharedPointer<WorkflowContext>& pContext)> mGeneratorController;
		const std::function<QSharedPointer<WorkflowContext>()> mGeneratorContext;
		const BusyHandler mBusyHandler;
		const QVariant mData;

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
		template<typename Controller, typename Context, typename ... Args>
		static QSharedPointer<WorkflowRequest> create(Args&& ... pArgs)
		{
			auto [controller, context] = getGenerator<Controller, Context, Args...>(std::forward<Args>(pArgs) ...);
			return QSharedPointer<WorkflowRequest>::create(controller, context);
		}


		template<typename Controller, typename Context, typename ... Args>
		static QSharedPointer<WorkflowRequest> createHandler(const BusyHandler& pBusyHandler, Args&& ... pArgs)
		{
			return createHandler<Controller, Context, Args...>(pBusyHandler, QVariant(), std::forward<Args>(pArgs) ...);
		}


		template<typename Controller, typename Context, typename ... Args>
		static QSharedPointer<WorkflowRequest> createHandler(const BusyHandler& pBusyHandler, const QVariant& pData, Args&& ... pArgs)
		{
			auto [controller, context] = getGenerator<Controller, Context, Args...>(std::forward<Args>(pArgs) ...);
			return QSharedPointer<WorkflowRequest>::create(controller, context, pBusyHandler, pData);
		}


		WorkflowRequest(const std::function<QSharedPointer<WorkflowController>(const QSharedPointer<WorkflowContext>& pContext)>& pGeneratorController,
				const std::function<QSharedPointer<WorkflowContext>()>& pGeneratorContext,
				const BusyHandler& pHandler = BusyHandler(),
				const QVariant& pData = QVariant());

		void initialize();
		[[nodiscard]] bool isInitialized() const;
		[[nodiscard]] Action getAction() const;
		[[nodiscard]] QSharedPointer<WorkflowController> getController() const;
		[[nodiscard]] QSharedPointer<WorkflowContext> getContext() const;
		[[nodiscard]] QVariant getData() const;
		[[nodiscard]] WorkflowControl handleBusyWorkflow(const QSharedPointer<WorkflowRequest>& pActiveWorkflow, const QSharedPointer<WorkflowRequest>& pWaitingWorkflow) const;
};

} // namespace governikus
