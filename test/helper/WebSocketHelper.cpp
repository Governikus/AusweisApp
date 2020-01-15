/*!
 * \copyright Copyright (c) 2016-2020 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketHelper.h"

#include "UIPlugInWebSocket.h"

#include <QThread>
#include <QTimer>

using namespace governikus;

void WebSocketHelper::connectWebsocket(int pPort)
{
	QEventLoop eventLoop;

	connect(&mWebSocket, &QWebSocket::connected, &eventLoop, &QEventLoop::quit);
	connect(&mWebSocket, &QWebSocket::disconnected, &eventLoop, &QEventLoop::quit);
	connect(&mWebSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), &eventLoop, &QEventLoop::quit);

	const QString address = QStringLiteral("ws://localhost:%1/eID-Kernel").arg(pPort);
	mWebSocket.open(QUrl(address));

	QTimer timer;
	timer.setSingleShot(true);
	timer.setInterval(mConnectionTiemout);
	connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);

	eventLoop.exec();
}


void WebSocketHelper::onTextMessageReceived(const QString& pMessage)
{
	mInput += pMessage;
}


WebSocketHelper::WebSocketHelper(int pPort, int pConnectionTimeout)
	: mConnectionTiemout(pConnectionTimeout)
{
	connect(&mWebSocket, &QWebSocket::textMessageReceived, this, &WebSocketHelper::onTextMessageReceived);

	const qint64 start = QDateTime::currentMSecsSinceEpoch();
	do
	{
		connectWebsocket(pPort);
	}
	while (mWebSocket.state() != QAbstractSocket::SocketState::ConnectedState
	&& mWebSocket.error() == QAbstractSocket::SocketError::ConnectionRefusedError
	&& QDateTime::currentMSecsSinceEpoch() - start < mConnectionTiemout);
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

		QTimer timer;
		timer.setSingleShot(true);
		timer.setInterval(mConnectionTiemout);
		connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);

		eventLoop.exec();
	}
	while (QDateTime::currentMSecsSinceEpoch() - start < mConnectionTiemout);

	return false;
}


void WebSocketHelper::sendMessage(const QString& pMessage)
{
	mWebSocket.sendTextMessage(pMessage);
}
