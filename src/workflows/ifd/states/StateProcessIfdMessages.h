/**
 * Copyright (c) 2017-2024 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief This state allows the processing of ordenary remote messages in the
 * background and handles special PACE messages.
 */

#pragma once


#include "ReaderManager.h"
#include "context/IfdServiceContext.h"
#include "states/AbstractState.h"
#include "states/GenericContextContainer.h"

class test_StateProcessIfdMessages;

namespace governikus
{

class StateProcessIfdMessages
	: public AbstractState
	, public GenericContextContainer<IfdServiceContext>
{
	Q_OBJECT
	friend class StateBuilder;
	friend class ::test_StateProcessIfdMessages;

	private:
		bool mResetContextOnDisconnect;
		QList<QMetaObject::Connection> mMessageConnections;

		explicit StateProcessIfdMessages(const QSharedPointer<WorkflowContext>& pContext);
		void run() override;

	private Q_SLOTS:
		void onMessageHandlerAdded(const QSharedPointer<ServerMessageHandler>& pHandler);
		void onClosed();
		void onConnectedChanged(bool pConnected) const;
		void onReaderPropertiesUpdated(const ReaderInfo& pInfo) const;
		void onCardConnected();
		void onDisplayTextChanged(const QString& pDisplayText) const;
		void onModifyPin(const QSharedPointer<const IfdModifyPin>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void onEstablishPaceChannel(const QSharedPointer<const IfdEstablishPaceChannel>& pMessage, const QSharedPointer<CardConnection>& pConnection);
		void onCardDisconnected() const;

	protected:
		void onExit(QEvent* pEvent) override;

	public:
		~StateProcessIfdMessages() override = default;

	Q_SIGNALS:
		void fireEstablishPaceChannel();
		void fireModifyPin();
		void fireSecureMessagingStopped();
};

} // namespace governikus
