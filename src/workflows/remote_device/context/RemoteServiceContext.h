/*!
 * \brief Remote service context.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "context/WorkflowContext.h"
#include "EstablishPaceChannel.h"
#include "EstablishPaceChannelOutput.h"
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

		QString mSlotHandle;
		EstablishPaceChannel mEstablishPaceChannel;
		int mPreferredPinLength;
		EstablishPaceChannelOutput mEstablishPaceChannelOutput;

		QSharedPointer<const IfdModifyPin> mModifyPinMessage;
		ResponseApdu mModifyPinMessageResponseApdu;

		[[nodiscard]] bool isPaceRequestingRights() const;

	public Q_SLOTS:
		void onMessageHandlerAdded(QSharedPointer<ServerMessageHandler> pHandler);

	Q_SIGNALS:
		void fireCardConnectionEstablished(const QSharedPointer<CardConnection>& pConnection);
		void fireCancelPasswordRequest();
		void fireEstablishPaceChannelUpdated();
		void fireIsRunningChanged();

	public:
		RemoteServiceContext();
		~RemoteServiceContext() override;

		[[nodiscard]] const QSharedPointer<RemoteServer>& getRemoteServer() const;
		[[nodiscard]] bool isRunning() const;

		[[nodiscard]] const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		[[nodiscard]] bool isPinChangeWorkflow() const;
		[[nodiscard]] bool isCanAllowedMode() const override;

		void setEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		[[nodiscard]] const QString& getSlotHandle() const;
		[[nodiscard]] const EstablishPaceChannel& getEstablishPaceChannel() const;
		[[nodiscard]] int getPreferredPinLength() const;

		void setEstablishPaceChannelOutput(const EstablishPaceChannelOutput& pEstablishPaceChannelOutput);
		[[nodiscard]] const EstablishPaceChannelOutput& getEstablishPaceChannelOutput() const;

		void setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage);
		[[nodiscard]] const QSharedPointer<const IfdModifyPin>& getModifyPinMessage() const;

		void setModifyPinMessageResponseApdu(const ResponseApdu& pModifyPinMessageResponseApdu);
		[[nodiscard]] const ResponseApdu& getModifyPinMessageResponseApdu() const;

		void resetPacePasswords() override;

	public Q_SLOTS:
		void onResetMessageHandler();
};

} // namespace governikus
