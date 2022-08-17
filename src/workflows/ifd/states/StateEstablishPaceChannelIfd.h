/*!
 * \brief This state executes the remote message PACE channel establish.
 *
 * \copyright Copyright (c) 2017-2022 Governikus GmbH & Co. KG, Germany
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
		PacePasswordId mPasswordId;

		explicit StateEstablishPaceChannelIfd(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
