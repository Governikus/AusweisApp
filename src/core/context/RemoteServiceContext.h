/*!
 * \brief Remote service context.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "messages/IfdEstablishPaceChannel.h"
#include "messages/IfdModifyPin.h"
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

		QString mNewPin;
		QSharedPointer<const IfdEstablishPaceChannel> mEstablishPaceChannelMessage;
		QSharedPointer<const IfdModifyPin> mModifyPinMessage;

	Q_SIGNALS:
		void fireCancelPasswordRequest();

	public:
		RemoteServiceContext();
		virtual ~RemoteServiceContext();

		const QSharedPointer<RemoteServer>& getRemoteServer() const;
		bool isRunning() const;

		const QString& getNewPin() const;
		void setNewPin(const QString& pNewPin);

		void setEstablishPaceChannelMessage(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage);
		const QSharedPointer<const IfdEstablishPaceChannel>& getEstablishPaceChannelMessage() const;

		void setModifyPinMessage(const QSharedPointer<const IfdModifyPin>& pMessage);
		const QSharedPointer<const IfdModifyPin>& getModifyPinMessage() const;

	public Q_SLOTS:
		void onResetMessageHandler();
};

} /* namespace governikus */
