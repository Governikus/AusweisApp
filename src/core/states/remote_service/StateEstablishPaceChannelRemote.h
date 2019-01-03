/*!
 * \brief This state executes the remote message PACE channel establish.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

class test_StateEstablishPaceChannelRemote;

namespace governikus
{

class StateEstablishPaceChannelRemote
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateEstablishPaceChannelRemote;

	private:
		PacePasswordId mPasswordId;

		explicit StateEstablishPaceChannelRemote(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);
};

} // namespace governikus
