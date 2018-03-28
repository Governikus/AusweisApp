/*!
 * \brief This state executes the remote message PACE channel establish.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

namespace governikus
{


class StateEstablishPaceChannel
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;

	private:
		QSharedPointer<const IfdEstablishPaceChannel> mEstablishPaceChannelMessage;
		QSharedPointer<ServerMessageHandler> mMessageHandler;

		StateEstablishPaceChannel(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onCancelEstablishPaceChannel();
		void onReaderInfoChanged(const ReaderInfo& pReaderInfo);
		void onEstablishConnectionDone(QSharedPointer<BaseCardCommand> pCommand);

	protected:
		void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateEstablishPaceChannel() override;

		void onEntry(QEvent* pEvent) override;

};

} /* namespace governikus */
