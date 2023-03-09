/**
 * Copyright (c) 2016-2023 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketHelper.h"

#include "UIPlugInWebSocket.h"

#include <QThread>
#include <QTimer>

using namespace governikus;

void WebSocketHelper::connectWebsocket(int pPort)
{
	const QString address = QStringLiteral("ws://localhost:%1/eID-Kernel").arg(pPort);
	mWebSocket.open(QUrl(address));
}


void WebSocketHelper::onTextMessageReceived(const QString& pMessage)
{
	mInput += pMessage;
}


WebSocketHelper::WebSocketHelper(int pPort, int pConnectionTimeout)
	: mConnectionTiemout(pConnectionTimeout)
{
	connect(&mWebSocket, &QWebSocket::textMessageReceived, this, &WebSocketHelper::onTextMessageReceived);
	connectWebsocket(pPort);
}


bool WebSocketHelper::isConnected() const
{
	return mWebSocket.state() == QAbstractSocket::SocketState::ConnectedState;
}


QAbstractSocket::SocketState WebSocketHelper::getState() const
{
	return mWebSocket.state();
}


bool WebSocketHelper::waitForMessage(const std::function<bool(const QJsonObject&)>& pMessageMatcher)
{
	const qint64 start = QDateTime::currentMSecsSinceEpoch();

	do
	{
		while (!mInput.isEmpty())
		{
			const QString& message = mInput.takeFirst();
			const QJsonDocument& document = QJsonDocument::fromJson(message.toUtf8());

			if (pMessageMatcher(document.object()))
			{
				return true;
			}
		}

		QEventLoop eventLoop;
		connect(&mWebSocket, &QWebSocket::textMessageReceived, &eventLoop, &QEventLoop::quit);

		QTimer::singleShot(mConnectionTiemout, &eventLoop, &QEventLoop::quit);

		eventLoop.exec();
	}
	while (QDateTime::currentMSecsSinceEpoch() - start < mConnectionTiemout);

	return false;
}


void WebSocketHelper::sendMessage(const QString& pMessage)
{
	mWebSocket.sendTextMessage(pMessage);
}
