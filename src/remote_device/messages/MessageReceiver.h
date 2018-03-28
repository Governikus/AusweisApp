/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include <QSharedPointer>


namespace governikus
{
class RemoteMessage;
class IfdEstablishContext;
class IfdEstablishContextResponse;
class GetIfdStatus;
class ReaderListMsg;
class IfdStatus;
class IfdConnect;
class IfdConnectResponse;
class IfdDisconnect;
class IfdDisconnectResponse;
class IfdError;
class IfdTransmit;
class IfdTransmitResponse;
class IfdEstablishPaceChannel;
class IfdEstablishPaceChannelResponse;
class IfdModifyPin;
class IfdModifyPinResponse;
class RemoteDispatcher;

class MessageReceiver
{
	Q_DISABLE_COPY(MessageReceiver)

	public:
		MessageReceiver() = default;
		virtual ~MessageReceiver() = default;

		virtual void process(const QSharedPointer<const IfdEstablishContext>& pMessage);
		virtual void process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage);
		virtual void process(const QSharedPointer<const GetIfdStatus>& pMessage);
		virtual void process(const QSharedPointer<const IfdStatus>& pMessage);
		virtual void process(const QSharedPointer<const IfdConnect>& pMessage);
		virtual void process(const QSharedPointer<const IfdConnectResponse>& pMessage);
		virtual void process(const QSharedPointer<const IfdDisconnect>& pMessage);
		virtual void process(const QSharedPointer<const IfdDisconnectResponse>& pMessage);
		virtual void process(const QSharedPointer<const IfdError>& pMessage);
		virtual void process(const QSharedPointer<const IfdTransmit>& pMessage);
		virtual void process(const QSharedPointer<const IfdTransmitResponse>& pMessage);
		virtual void process(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		virtual void process(const QSharedPointer<const IfdEstablishPaceChannelResponse>& pMessage);
		virtual void process(const QSharedPointer<const IfdModifyPin>& pMessage);
		virtual void process(const QSharedPointer<const IfdModifyPinResponse>& pMessage);

		virtual void unprocessed(const QSharedPointer<const RemoteMessage>& pMessage);

		void receive(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher = QSharedPointer<RemoteDispatcher>());
};


} /* namespace governikus */
