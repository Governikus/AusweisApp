/**
 * Copyright (c) 2016-2025 Governikus GmbH & Co. KG, Germany
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

		void handleNpaPosition(CardReturnCode pReturnCode) const;

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void fireNoCardConnection();
		void fireWrongPassword();
		void fireThirdPinAttemptFailed();
		void fireAbortAndUnfortunateCardPosition();
		void firePaceChannelFailed();
		void firePaceCanEstablished();
		void firePacePukEstablished();
};

} // namespace governikus
