/*!
 * \brief Template base class for all steps taken by the state machine.
 *        It is parameterized over the model type and contains a getter for the model.
 *        We cannot parameterize the super class AbstractState because Qt does not
 *        support template classes.
 *
 * \copyright Copyright (c) 2014-2020 Governikus GmbH & Co. KG, Germany
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


		virtual ~GenericContextContainer() = default;

		virtual QSharedPointer<ContextClass> getContext()
		{
			return mTypedContext;
		}


};

} // namespace governikus
