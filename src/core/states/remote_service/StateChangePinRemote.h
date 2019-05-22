/*!
 * \brief This state executes the remote message IfdModifyPin.
 *
 * \copyright Copyright (c) 2017-2019 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

class test_StateChangePinRemote;

namespace governikus
{

class StateChangePinRemote
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangePinRemote;

	private:
		explicit StateChangePinRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onChangePinDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
