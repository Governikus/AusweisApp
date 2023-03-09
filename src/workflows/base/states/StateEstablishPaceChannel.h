/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Controller for the step that tries to establish a PACE
 *        connection using the card's Pin.
 */

#pragma once

#include "AbstractState.h"
#include "GenericContextContainer.h"
#include "context/WorkflowContext.h"

class test_StateEstablishPaceChannel;

namespace governikus
{

class StateEstablishPaceChannel
	: public AbstractState
	, public GenericContextContainer<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEstablishPaceChannel;

	private:
		explicit StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext);

		PacePasswordId mPasswordId;

		void run() override;
		void onUserCancelled() override;

		void abort(FailureCode::Reason pReason);
		void abortToChangePin(FailureCode::Reason pReason);
		void handleNpaPosition(CardReturnCode pReturnCode);

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void firePaceChannelEstablished();
		void firePaceChannelInoperative();
		void fireAbortAndUnfortunateCardPosition();
		void firePropagateAbort();
};

} // namespace governikus
