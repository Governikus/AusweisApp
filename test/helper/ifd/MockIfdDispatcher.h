/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "IfdDispatcherClient.h"


namespace governikus
{

class MockIfdDispatcher
	: public IfdDispatcherClient
{
	Q_OBJECT

	public:
		enum class DispatcherState
		{
			WithoutReader,
			ReaderInvalid,
			ReaderWithoutCard,
			ReaderWithCard,
			ReaderWithCardError
		};

	private:
		DispatcherState mState;
		bool mPairing;
		QByteArray mId;
		QString mContextHandle;

	public:
		MockIfdDispatcher(DispatcherState pState = DispatcherState::WithoutReader);

		void setPairingConnection(bool pPairing);
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] QByteArray getId() const override;
		[[nodiscard]] const QString& getContextHandle() const override;
		Q_INVOKABLE void send(const QSharedPointer<const IfdMessage>& pMessage) override;

		[[nodiscard]] DispatcherState getState() const;
		void setState(DispatcherState pState);
		void onClosed();

	public Q_SLOTS:
		void onReceived(const QSharedPointer<const IfdMessage>& pMessage);

	Q_SIGNALS:
		void fireSend(const QSharedPointer<const IfdMessage>& pMessage);

};


} // namespace governikus
