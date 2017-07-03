/*!
 * \copyright Copyright (c) 2017 Governikus GmbH & Co. KG
 */

#include "WebSocketChannel.h"


using namespace governikus;


WebSocketChannel::WebSocketChannel(const QSharedPointer<QWebSocket>& pConnection)
	: mConnection(pConnection)
{
	if (mConnection)
	{
		connect(mConnection.data(), &QWebSocket::textMessageReceived, this, &WebSocketChannel::onReceived);
		connect(mConnection.data(), &QWebSocket::disconnected, this, &WebSocketChannel::onDisconnected);
	}
}


WebSocketChannel::~WebSocketChannel()
{
	if (mConnection)
	{
		disconnect(mConnection.data(), &QWebSocket::textMessageReceived, this, &WebSocketChannel::onReceived);
		disconnect(mConnection.data(), &QWebSocket::disconnected, this, &WebSocketChannel::onDisconnected);
	}
}


void WebSocketChannel::send(const QByteArray& pDataBlock)
{
	if (mConnection)
	{
		mConnection->sendTextMessage(QString::fromUtf8(pDataBlock));
	}
}


void WebSocketChannel::close()
{
	if (mConnection)
	{
		mConnection->close();
	}
}


void WebSocketChannel::onReceived(const QString& pMessage)
{
	Q_EMIT fireReceived(pMessage.toUtf8());
}


void WebSocketChannel::onDisconnected()
{
	Q_EMIT fireClosed(mConnection->closeCode() == QWebSocketProtocol::CloseCodeNormal ?
			GlobalStatus::Code::RemoteReader_CloseCode_NormalClose :
			GlobalStatus::Code::RemoteReader_CloseCode_AbnormalClose);
}
