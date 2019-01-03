/*!
 * \brief Implementation of DataChannel base on web sockets.
 *
 * \copyright Copyright (c) 2017-2018 Governikus GmbH & Co. KG, Germany
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
		const QString mId;
		QTimer mPingTimer;
		QTimer mPongTimer;
		static QString makeConnectionId(const QSharedPointer<QWebSocket>& pConnection);

	public:
		WebSocketChannel(const QSharedPointer<QWebSocket>& pConnection);
		virtual ~WebSocketChannel() override;

		virtual void send(const QByteArray& pDataBlock) override;
		virtual void close() override;
		virtual const QString& getId() const override;

	private Q_SLOTS:
		void onReceived(const QString& pMessage);
		void onDisconnected();
		void onPingScheduled();
		void onPongReceived();
		void onPongTimeout();
};

} // namespace governikus
