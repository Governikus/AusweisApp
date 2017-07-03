/*!
 * WebSocketChannel.h
 *
 * \brief Implementation of DataChannel base on web sockets.
 *
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#pragma once

#include "DataChannel.h"

#include <QByteArray>
#include <QObject>
#include <QWebSocket>

namespace governikus
{
class WebSocketChannel
	: public DataChannel
{
	Q_OBJECT

	private:
		const QSharedPointer<QWebSocket> mConnection;

	public:
		WebSocketChannel(const QSharedPointer<QWebSocket>& pConnection);
		virtual ~WebSocketChannel();

		virtual void send(const QByteArray& pDataBlock) override;
		virtual void close() override;

	private Q_SLOTS:
		void onReceived(const QString& pMessage);
		void onDisconnected();

};

} /* namespace governikus */
