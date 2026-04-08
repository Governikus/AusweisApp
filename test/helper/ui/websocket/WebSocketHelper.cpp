/**
 * Copyright (c) 2016-2026 Governikus GmbH & Co. KG, Germany
 */

#include "WebSocketHelper.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>


using namespace governikus;


void WebSocketHelper::connectWebsocket(int pPort)
{
	const QUrl address(QStringLiteral("ws://localhost:%1/eID-Kernel").arg(pPort));
	QNetworkRequest request(address);
	if (!mOrigin.isNull())
	{
		request.setRawHeader("origin", mOrigin);
	}
	mWebSocket.open(request);
}


void WebSocketHelper::onTextMessageReceived(const QString& pMessage)
{
	mInput += pMessage;
}


WebSocketHelper::WebSocketHelper(int pPort, const QByteArray& pOrigin, int pConnectionTimeout)
	: mConnectionTimeout(pConnectionTimeout)
	, mInput()
	, mError(QAbstractSocket::UnknownSocketError)
	, mOrigin(pOrigin)
{
	connect(&mWebSocket, &QWebSocket::errorOccurred, this, [this](QAbstractSocket::SocketError pError) {
				mError = pError;
			});
	connect(&mWebSocket, &QWebSocket::textMessageReceived, this, &WebSocketHelper::onTextMessageReceived);
	connectWebsocket(pPort);
}


bool WebSocketHelper::isConnected() const
{
	return mWebSocket.state() == QAbstractSocket::SocketState::ConnectedState;
}


QAbstractSocket::SocketError WebSocketHelper::getError() const
{
	return mError;
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

		QTimer::singleShot(mConnectionTimeout, &eventLoop, &QEventLoop::quit);

		eventLoop.exec();
	}
	while (QDateTime::currentMSecsSinceEpoch() - start < mConnectionTimeout);

	return false;
}


void WebSocketHelper::sendMessage(const QString& pMessage)
{
	mWebSocket.sendTextMessage(pMessage);
}
