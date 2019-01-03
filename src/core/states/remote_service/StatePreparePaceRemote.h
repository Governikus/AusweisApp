/*!
 * \copyright Copyright (c) 2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"


namespace governikus
{

class StatePreparePaceRemote
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		explicit StatePreparePaceRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	Q_SIGNALS:
		void fireEnterPacePassword();
};

} // namespace governikus
