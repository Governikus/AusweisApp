/*!
 * \brief Remote dispatcher mock for tests.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "RemoteDispatcher.h"

namespace governikus
{

class MockRemoteDispatcher
	: public RemoteDispatcher
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
		virtual ~MockRemoteDispatcher() override = default;

		virtual const QString& getId() const override;
		virtual const QString& getContextHandle() const override;
		virtual void send(const QSharedPointer<const RemoteMessage>& pMessage) override;

		DispatcherState getState() const;
		void setState(DispatcherState pState);
		void onClosed();

	public Q_SLOTS:
		void onReceived(const QSharedPointer<const RemoteMessage>& pMessage);

	Q_SIGNALS:
		void fireSend(const QSharedPointer<const RemoteMessage>& pMessage);

};


} /* namespace governikus */
