/*!
 * \brief Remote dispatcher mock for tests.
 *
 * \copyright Copyright (c) 2017-2021 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteDispatcherClient.h"


namespace governikus
{

class MockRemoteDispatcher
	: public RemoteDispatcherClient
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
		QString mId;
		QString mContextHandle;

	public:
		MockRemoteDispatcher(DispatcherState pState = DispatcherState::WithoutReader);

		[[nodiscard]] QString getId() const override;
		[[nodiscard]] const QString& getContextHandle() const override;
		Q_INVOKABLE void send(const QSharedPointer<const RemoteMessage>& pMessage) override;

		[[nodiscard]] DispatcherState getState() const;
		void setState(DispatcherState pState);
		void onClosed();

	public Q_SLOTS:
		void onReceived(const QSharedPointer<const RemoteMessage>& pMessage);

	Q_SIGNALS:
		void fireSend(const QSharedPointer<const RemoteMessage>& pMessage);

};


} // namespace governikus
