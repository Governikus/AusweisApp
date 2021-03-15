/*!
 * \brief This state executes the remote message IfdModifyPin.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateChangePinRemote;

namespace governikus
{

class StateChangePinRemote
	: public AbstractState
	, public GenericContextContainer<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangePinRemote;

	private:
		explicit StateChangePinRemote(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onChangePinDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
