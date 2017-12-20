/*!
 * \brief Helper class to verify the content of remote messages.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/MessageReceiver.h"


namespace governikus
{
class Discovery;


class RemoteMessageChecker
	: public MessageReceiver
{
	public:
		RemoteMessageChecker() = default;
		virtual ~RemoteMessageChecker() override = default;

		void processDiscovery(const QSharedPointer<const Discovery>& pMessage);
		virtual void process(const QSharedPointer<const IfdEstablishContext>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage) override;
		virtual void process(const QSharedPointer<const GetIfdStatus>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdStatus>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdConnect>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdConnectResponse>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdDisconnect>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdDisconnectResponse>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdError>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdTransmit>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdTransmitResponse>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage) override;
		virtual void process(const QSharedPointer<const IfdEstablishPaceChannelResponse>& pMessage) override;

};

} /* namespace governikus */
