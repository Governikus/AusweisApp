/*!
 * \brief Remote service context.
 *
 * \copyright Copyright (c) 2017-2020 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "EstablishPaceChannelOutput.h"
#include "EstablishPaceChannelParser.h"
#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdModifyPin.h"
#include "RemoteServer.h"

#include <QSharedPointer>


namespace governikus
{

class RemoteServiceContext
	: public WorkflowContext
{
	Q_OBJECT

	private:
		const QSharedPointer<RemoteServer> mRemoteServer;

		QString mNewPin;
		QSharedPointer<const IfdEstablishPaceChannel> mEstablishPaceChannelMessage;
		EstablishPaceChannelOutput mEstablishPaceChannelOutput;
		QSharedPointer<const IfdModifyPin> mModifyPinMessage;
		ResponseApdu mModifyPinMessageResponseApdu;
		EstablishPaceChannelParser mPaceChannelParser;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);

	Q_SIGNALS:
		void fireCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection);
		void fireCancelPasswordRequest();
		void fireEstablishPaceChannelMessageUpdated(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);

	public:
		RemoteServiceContext();
		virtual ~RemoteServiceContext() override;

		const QSharedPointer<RemoteServer>& getRemoteServer() const;
		bool isRunning() const;

		const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		bool isCanAllowedMode() const override;

		void setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		bool hasEstablishedPaceChannelRequest() const;
		QString getEstablishPaceChannelMessageSlotHandle() const;
		const EstablishPaceChannelParser& getPaceChannelParser() const;

		void setEstablishPaceChannelOutput(const EstablishPaceChannelOutput& pEstablishPaceChannelOutput);
		const EstablishPaceChannelOutput& getEstablishPaceChannelOutput() const;

		void setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage);
		const QSharedPointer<const IfdModifyPin>& getModifyPinMessage() const;

		void setModifyPinMessageResponseApdu(const ResponseApdu& pModifyPinMessageResponseApdu);
		const ResponseApdu& getModifyPinMessageResponseApdu() const;

		void resetPacePasswords() override;

	public Q_SLOTS:
		void onResetMessageHandler();
};

} // namespace governikus
