/*!
 * \brief Template base class for all steps taken by the state machine.
 *        It is parameterized over the model type and contains a getter for the model.
 *        We cannot parameterize the super class AbstractState because Qt does not
 *        support template classes.
 *
 * \copyright Copyright (c) 2014-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"

#include <QLoggingCategory>


namespace governikus
{

template<typename ContextClass>
class AbstractGenericState
	: public AbstractState
{
	public:
		explicit AbstractGenericState(const QSharedPointer<WorkflowContext>& pContext, bool pConnectOnCardRemoved = true)
			: AbstractState(pContext, pConnectOnCardRemoved)
		{
			Q_ASSERT(mContext.objectCast<ContextClass>());
		}


		virtual ~AbstractGenericState()
		{
		}


		virtual QSharedPointer<ContextClass> getContext()
		{
			return mContext.staticCast<ContextClass>();
		}


};

} // namespace governikus
