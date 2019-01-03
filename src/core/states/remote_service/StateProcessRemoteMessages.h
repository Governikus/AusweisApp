/*!
 * \brief This state allows the processing of ordenary remote messages in the
 * background and handles special PACE messages.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
 */

#pragma once


#include "context/RemoteServiceContext.h"
#include "states/AbstractGenericState.h"

class test_StateProcessRemoteMessages;

namespace governikus
{

class StateProcessRemoteMessages
	: public AbstractGenericState<RemoteServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateProcessRemoteMessages;

	private:
		QVector<QMetaObject::Connection> mMessageConnections;

		explicit StateProcessRemoteMessages(const QSharedPointer<WorkflowContext>& pContext);
		virtual void run() override;

	private Q_SLOTS:
		void onMessageHandlerAdded(const QSharedPointer<ServerMessageHandler>& pHandler);
		void onClosed();
		void onModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection);

	protected:
		void onExit(QEvent* pEvent) override;

	public:
		virtual ~StateProcessRemoteMessages() override;

	Q_SIGNALS:
		void fireEstablishPaceChannel();
		void fireModifyPin();
		void fireSecureMessagingStopped();
};

} // namespace governikus
