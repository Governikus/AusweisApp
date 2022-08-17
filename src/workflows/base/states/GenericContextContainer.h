/*!
 * \brief Template base class for all steps taken by the state machine.
 *        It is parameterized over the context type and contains a getter for the context.
 *        We cannot parameterize the super class AbstractState because Qt does not
 *        support template classes.
 *
 * \copyright Copyright (c) 2014-2022 Governikus GmbH & Co. KG, Germany
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
