/*!
 * \brief Handler for messages on the server side of a smartphone as card reader (SaC) scenario.
 *
 * \copyright Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "CardConnection.h"
#include "apdu/ResponseApdu.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdModifyPin.h"
#include "pinpad/EstablishPaceChannelOutput.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>


namespace governikus
{

class ServerMessageHandler
	: public QObject
{
	Q_OBJECT

	protected:
		ServerMessageHandler() = default;

	public:
		~ServerMessageHandler() override;

		virtual void sendEstablishPaceChannelResponse(const QString& pSlotHandle, const EstablishPaceChannelOutput&) = 0;
		virtual void sendModifyPinResponse(const QString& pSlotHandle, const ResponseApdu& pResponseApdu) = 0;

	Q_SIGNALS:
		void fireCardConnected(const QSharedPointer<CardConnection>& pConnection);
		void fireEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void fireModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void fireCardDisconnected(const QSharedPointer<CardConnection>& pConnection);
		void fireClosed();
		void fireSecureMessagingStopped();
};

} // namespace governikus
