/*!
 * \brief Remote service context.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/IfdEstablishPaceChannel.h"
#include "RemoteServer.h"
#include "SelfAuthenticationData.h"
#include "WorkflowContext.h"

#include <QSharedPointer>

namespace governikus
{

class RemoteServiceContext
	: public WorkflowContext
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteServer> mRemoteServer;

		QSharedPointer<const IfdEstablishPaceChannel> mEstablishPaceChannelMessage;

	Q_SIGNALS:
		void fireCancelEstablishPaceChannel();

	public:
		RemoteServiceContext();
		virtual ~RemoteServiceContext();

		const QSharedPointer<RemoteServer>& getRemoteServer() const;
		bool isRunning() const;

		void setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		const QSharedPointer<const IfdEstablishPaceChannel>& getEstablishPaceChannelMessage() const;

	public Q_SLOTS:
		void onResetMessageHandler();
};

} /* namespace governikus */
