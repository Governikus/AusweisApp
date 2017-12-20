/*!
 * \brief Template base class for all steps taken by the state machine.
 *        It is parameterized over the model type and contains a getter for the model.
 *        We cannot parameterize the super class AbstractState because Qt does not
 *        support template classes.
 *
 * \copyright Copyright (c) 2014-2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractState.h"

#include <QLoggingCategory>


namespace governikus
{

template<typename ModelClass>
class AbstractGenericState
	: public AbstractState
{
	public:
		AbstractGenericState(const QSharedPointer<WorkflowContext>& pContext, bool pConnectOnCardRemoved = true)
			: AbstractState(pContext, pConnectOnCardRemoved)
		{
			Q_ASSERT(mContext.objectCast<ModelClass>());
		}


		virtual ~AbstractGenericState()
		{
		}


		virtual QSharedPointer<ModelClass> getContext()
		{
			return mContext.staticCast<ModelClass>();
		}


};

} /* namespace governikus */
