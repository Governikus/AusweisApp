/*!
 * \brief Controller for the step that tries to establish a PACE
 *        connection using the card's Pin.
 *
 * \copyright Copyright (c) 2016-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "AbstractGenericState.h"
#include "context/WorkflowContext.h"

class test_StateEstablishPaceChannel;

namespace governikus
{

class StateEstablishPaceChannel
	: public AbstractGenericState<WorkflowContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEstablishPaceChannel;

	private:
		explicit StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext);

		PacePasswordId mPasswordId;

		virtual void run() override;
		virtual void onUserCancelled() override;

		void abort();
		void handleNpaPosition(CardReturnCode pReturnCode);

	private Q_SLOTS:
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	Q_SIGNALS:
		void firePaceChannelEstablished();
		void firePacePukEstablished();
		void fireAbortAndUnfortunateCardPosition();
};

} // namespace governikus
