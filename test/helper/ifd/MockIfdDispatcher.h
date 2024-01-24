/**
 * Copyright (c) 2017-2023 Governikus GmbH & Co. KG, Germany
 */

/*!
 * \brief Ifd dispatcher mock for tests.
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
			ReaderWithoutCard,
			ReaderWithCard,
			ReaderWithCardError
		};

	private:
		DispatcherState mState;
		bool mPairing;
		QString mId;
		QString mContextHandle;

	public:
		MockIfdDispatcher(DispatcherState pState = DispatcherState::WithoutReader);

		void setPairingConnection(bool pPairing);
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] QString getId() const override;
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
