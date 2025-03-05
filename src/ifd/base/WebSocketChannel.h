/**
 * Copyright (c) 2017-2025 Governikus GmbH & Co. KG, Germany
 */

#pragma once

#include "DataChannel.h"

#include <QByteArray>
#include <QObject>
#include <QTimer>
#include <QWebSocket>


namespace governikus
{
class WebSocketChannel
	: public DataChannel
{
	Q_OBJECT

	private:
		const QSharedPointer<QWebSocket> mConnection;
		const QByteArray mId;
		QTimer mPingTimer;
		QTimer mPongTimer;
		static QByteArray makeConnectionId(const QSharedPointer<QWebSocket>& pConnection);

	public:
		explicit WebSocketChannel(const QSharedPointer<QWebSocket>& pConnection);
		~WebSocketChannel() override;

		void send(const QByteArray& pDataBlock) override;
		void close() override;
		[[nodiscard]] bool isPairingConnection() const override;
		[[nodiscard]] const QByteArray& getId() const override;

	private Q_SLOTS:
		void onReceived(const QString& pMessage);
		void onDisconnected();
		void onPingScheduled();
		void onPongReceived();
		void onPongTimeout();
};

} // namespace governikus
