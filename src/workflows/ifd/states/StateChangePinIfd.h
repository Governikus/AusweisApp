/*!
 * \brief This state executes the remote message IfdModifyPin.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateChangePinIfd;

namespace governikus
{

class StateChangePinIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateChangePinIfd;

	private:
		explicit StateChangePinIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onChangePinDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
