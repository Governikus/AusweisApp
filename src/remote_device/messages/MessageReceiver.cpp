/*!
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#include "MessageReceiver.h"

#include "Discovery.h"
#include "GetIfdStatus.h"
#include "IfdConnect.h"
#include "IfdConnectResponse.h"
#include "IfdDisconnect.h"
#include "IfdDisconnectResponse.h"
#include "IfdError.h"
#include "IfdEstablishContext.h"
#include "IfdEstablishContextResponse.h"
#include "IfdEstablishPaceChannel.h"
#include "IfdEstablishPaceChannelResponse.h"
#include "IfdModifyPin.h"
#include "IfdModifyPinResponse.h"
#include "IfdStatus.h"
#include "IfdTransmit.h"
#include "IfdTransmitResponse.h"


using namespace governikus;


void MessageReceiver::process(const QSharedPointer<const IfdEstablishContext>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdEstablishContextResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const GetIfdStatus>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdStatus>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdConnect>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdConnectResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdDisconnect>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdDisconnectResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdError>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdTransmit>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdTransmitResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdEstablishPaceChannelResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdModifyPin>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::process(const QSharedPointer<const IfdModifyPinResponse>& pMessage)
{
	unprocessed(pMessage);
}


void MessageReceiver::unprocessed(const QSharedPointer<const RemoteMessage>& /*pMessage*/)
{
}


void MessageReceiver::receive(const QSharedPointer<const RemoteMessage>& pMessage, const QSharedPointer<RemoteDispatcher>& pRemoteDispatcher)
{
	Q_ASSERT(pMessage);
	Q_UNUSED(pRemoteDispatcher);

	switch (pMessage->getType())
	{
		case RemoteCardMessageType::IFDEstablishContext:
		{
			const QSharedPointer<const IfdEstablishContext> castMessage = pMessage.dynamicCast<const IfdEstablishContext>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDEstablishContextResponse:
		{
			const QSharedPointer<const IfdEstablishContextResponse> castMessage = pMessage.dynamicCast<const IfdEstablishContextResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDGetStatus:
		{
			const QSharedPointer<const GetIfdStatus> castMessage = pMessage.dynamicCast<const GetIfdStatus>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDStatus:
		{
			const QSharedPointer<const IfdStatus> castMessage = pMessage.dynamicCast<const IfdStatus>();
			process(castMessage);
			break;
		}


		case RemoteCardMessageType::IFDConnect:
		{
			const QSharedPointer<const IfdConnect> castMessage = pMessage.dynamicCast<const IfdConnect>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDConnectResponse:
		{
			const QSharedPointer<const IfdConnectResponse> castMessage = pMessage.dynamicCast<const IfdConnectResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDDisconnect:
		{
			const QSharedPointer<const IfdDisconnect> castMessage = pMessage.dynamicCast<const IfdDisconnect>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDDisconnectResponse:
		{
			const QSharedPointer<const IfdDisconnectResponse> castMessage = pMessage.dynamicCast<const IfdDisconnectResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDError:
		{
			const QSharedPointer<const IfdError> castMessage = pMessage.dynamicCast<const IfdError>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDTransmit:
		{
			const QSharedPointer<const IfdTransmit> castMessage = pMessage.dynamicCast<const IfdTransmit>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDTransmitResponse:
		{
			const QSharedPointer<const IfdTransmitResponse> castMessage = pMessage.dynamicCast<const IfdTransmitResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDEstablishPACEChannel:
		{
			const QSharedPointer<const IfdEstablishPaceChannel> castMessage = pMessage.dynamicCast<const IfdEstablishPaceChannel>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDEstablishPACEChannelResponse:
		{
			const QSharedPointer<const IfdEstablishPaceChannelResponse> castMessage = pMessage.dynamicCast<const IfdEstablishPaceChannelResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDModifyPIN:
		{
			const QSharedPointer<const IfdModifyPin> castMessage = pMessage.dynamicCast<const IfdModifyPin>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::IFDModifyPINResponse:
		{
			const QSharedPointer<const IfdModifyPinResponse> castMessage = pMessage.dynamicCast<const IfdModifyPinResponse>();
			process(castMessage);
			break;
		}

		case RemoteCardMessageType::UNDEFINED:
		{
			unprocessed(pMessage);
			break;
		}
	}
}
