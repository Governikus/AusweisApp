/**
 * Copyright (c) 2017-2026 Governikus Service GmbH, Germany
 */

#pragma once


#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"


class test_StateEstablishPaceChannelIfd;


namespace governikus
{

class StateEstablishPaceChannelIfd
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEstablishPaceChannelIfd;

	private:
		explicit StateEstablishPaceChannelIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
