/**
 * Copyright (c) 2014-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"


namespace governikus
{

template<typename ContextClass>
class GenericContextContainer
{
	private:
		const QSharedPointer<ContextClass> mTypedContext;

	public:
		explicit GenericContextContainer(const QSharedPointer<WorkflowContext>& pContext)
			: mTypedContext(pContext.staticCast<ContextClass>())
		{
			Q_ASSERT(pContext.objectCast<ContextClass>());
		}


		[[nodiscard]] QSharedPointer<ContextClass> getContext() const
		{
			return mTypedContext;
		}


};

} // namespace governikus
